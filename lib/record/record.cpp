#include "record/record.h"
#include "encoding.h"

#include <cassert>
#include <limits>

RecordManager::RecordManager() = default;
RecordManager::~RecordManager() = default;

void record::ValueSerializer::operator()(record::Null) {}

void record::ValueSerializer::operator()(record::Integer v) {
  encoding::append_uint64(buf, v);
}

void record::ValueSerializer::operator()(record::Bool v) {
  encoding::append_bool(buf, v);
}

void record::ValueSerializer::operator()(const record::Text &v) {
  assert(v.size() <= std::numeric_limits<uint32_t>::max());
  encoding::append_uint32(buf, static_cast<uint32_t>(v.size()));
  const std::byte *str_bytes = reinterpret_cast<const std::byte *>(v.data());
  buf.insert(buf.end(), str_bytes, str_bytes + v.size());
}

std::vector<record::Flag> RecordManager::extract_flags(uint16_t flags) {
  std::vector<record::Flag> result;
  if (flags & ROW_HASNULL)
    result.push_back(record::Flag::HASNULL);
  return result;
}

bool RecordManager::is_null(const std::byte *bitmap, uint16_t col_idx) {
  return static_cast<uint8_t>(bitmap[col_idx / 8]) & (1 << (col_idx % 8));
}

void RecordManager::set_bit(std::vector<std::byte> &nullbytes, size_t idx) {
  nullbytes[idx / 8] |= std::byte(1 << (idx % 8));
}

record::RowHeader RecordManager::build_row_header(record::Row &row,
                                                  schema::Table &table) {
  uint16_t flags = 0x0000;
  record::RowHeader row_header;
  row_header.xmax = row_header.xmin = 0;

  for (auto col : table.columns) {
    if (col.is_nullable)
      flags = (flags) | 0x0001;
  }
  row_header.flags = flags;
  return row_header;
}

record::Row RecordManager::deserialize(std::vector<std::byte> &bytes,
                                       schema::Table &table) {
  record::Row row;

  size_t offset = 0;
  uint64_t xmin = encoding::read_uint64(bytes, offset);
  uint64_t xmax = encoding::read_uint64(bytes, offset);
  uint16_t flagbits = encoding::read_uint16(bytes, offset);

  std::vector<record::Flag> flags = extract_flags(flagbits);
  std::byte *nullbits = nullptr;
  for (auto flag : flags) {
    if (flag == record::Flag::HASNULL) {
      nullbits = &bytes[offset];
      offset += (table.columns.size() + 7) / 8;
    }
  }

  for (uint16_t i = 0; i < table.columns.size(); i++) {
    if (nullbits != nullptr && is_null(nullbits, i)) {
      row.values.push_back(record::Null{});
      continue;
    }

    switch (table.columns[i].type) {
    case schema::DataType::BOOLEAN: {
      bool bool_val = static_cast<bool>(encoding::read_uint8(bytes, offset));
      row.values.push_back(bool_val);
      break;
    }
    case schema::DataType::INTEGER: {
      int64_t int_val =
          static_cast<int64_t>(encoding::read_uint64(bytes, offset));
      row.values.push_back(int_val);
      break;
    }
    case schema::DataType::TEXT: {
      uint32_t text_length = encoding::read_uint32(bytes, offset);
      record::VarlenHeader text_header = {.length = text_length,
                                          .data = &bytes[offset]};
      row.values.push_back(
          encoding::read_str(bytes, text_header.length, offset));
    }
    }
  }
  return row;
}

std::vector<std::byte> RecordManager::serialize(record::Row &row,
                                                schema::Table &table) {
  std::vector<std::byte> buffer;

  record::RowHeader row_header = build_row_header(row, table);
  size_t num_nullbytes = (table.columns.size() + 7) / 8;
  encoding::append_uint64(buffer, row_header.xmin);
  encoding::append_uint64(buffer, row_header.xmax);
  encoding::append_uint16(buffer, row_header.flags);

  std::vector<std::byte> nullbytes(num_nullbytes);
  std::vector<std::byte> values;
  assert(table.columns.size() == row.values.size());

  record::ValueSerializer visitor{values};
  for (size_t i = 0; i < row.values.size(); i++) {
    if (std::holds_alternative<record::Null>(row.values[i]) &&
        !table.columns[i].is_nullable) {
      throw std::runtime_error("[RecordManager]: NULL constraint violation");
    } else if (std::holds_alternative<record::Null>(row.values[i])) {
      set_bit(nullbytes, i);
    } else {
      assert(!std::holds_alternative<record::Null>(row.values[i]));
      std::visit(visitor, row.values[i]);
    }
  }
  if (row_header.flags & ROW_HASNULL)
    buffer.insert(buffer.end(), nullbytes.begin(), nullbytes.end());
  buffer.insert(buffer.end(), values.begin(), values.end());
  return buffer;
}
