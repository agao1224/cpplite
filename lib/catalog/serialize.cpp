#include <cassert>
#include <utility>

#include "catalog/catalog.h"
#include "catalog/schema.h"
#include "encoding.h"

std::vector<std::byte> CatalogManager::serialize_table(schema::Table table,
                                                       PageNumber root_pgno) {
  std::vector<std::byte> serialized;

  serialized.push_back(static_cast<std::byte>(table.type));

  uint16_t name_len = table.name.size();
  encoding::append_uint16(serialized, name_len);
  for (uint16_t i = 0; i < name_len; i++)
    serialized.push_back(static_cast<std::byte>(table.name[i]));

  uint16_t tbl_name_len = table.tbl_name.size();
  encoding::append_uint16(serialized, tbl_name_len);
  for (uint16_t i = 0; i < tbl_name_len; i++)
    serialized.push_back(static_cast<std::byte>(table.tbl_name[i]));

  encoding::append_uint64(serialized, static_cast<uint64_t>(root_pgno));
  uint16_t num_cols = table.columns.size();
  encoding::append_uint16(serialized, num_cols);
  for (uint16_t i = 0; i < num_cols; i++) {
    schema::Column col = table.columns[i];
    serialized.push_back(static_cast<std::byte>(col.type));

    uint16_t col_name_len = col.name.size();
    encoding::append_uint16(serialized, col_name_len);
    for (uint16_t j = 0; j < col_name_len; j++)
      serialized.push_back(static_cast<std::byte>(col.name[j]));

    serialized.push_back(static_cast<std::byte>(col.is_primary_key));
    serialized.push_back(static_cast<std::byte>(col.is_nullable));
  }
  return serialized;
}

std::pair<schema::Table, PageNumber>
CatalogManager::deserialize_table(std::vector<std::byte> &payload) {
  schema::Table table;
  size_t offset = 0;

  table.type = static_cast<schema::SchemaType>(payload[offset]);
  offset++;

  uint16_t name_len = encoding::read_uint16(payload, offset);
  std::string name(reinterpret_cast<const char *>(payload.data() + offset),
                   name_len);
  table.name = name;
  offset += name_len;

  uint16_t tbl_name_len = encoding::read_uint16(payload, offset);
  std::string tbl_name(reinterpret_cast<const char *>(payload.data() + offset),
                       tbl_name_len);
  table.tbl_name = tbl_name;
  offset += tbl_name_len;

  PageNumber root_pgno = static_cast<PageNumber>(encoding::read_uint64(payload, offset));

  uint16_t num_cols = encoding::read_uint16(payload, offset);
  for (uint16_t i = 0; i < num_cols; i++) {
    schema::Column col;
    col.type = static_cast<schema::DataType>(payload[offset]);
    offset++;

    uint16_t col_name_len = encoding::read_uint16(payload, offset);
    std::string col_name(
        reinterpret_cast<const char *>(payload.data() + offset), col_name_len);
    col.name = col_name;
    offset += col_name_len;

    col.is_primary_key = static_cast<bool>(payload[offset]);
    offset++;
    col.is_nullable = static_cast<bool>(payload[offset]);
    offset++;

    table.columns.push_back(col);
  }
  assert(payload.size() == offset);
  return {table, root_pgno};
}
