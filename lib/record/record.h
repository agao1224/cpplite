#include "catalog/schema.h"
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

static const uint16_t ROW_HASNULL = 0x0001;

namespace record {

enum Flag { HASNULL };

using Null = std::monostate;
using Integer = int64_t;
using Text = std::string;
using Bool = bool;

using Value = std::variant<Null, Integer, Text, Bool>;

struct RowHeader {
  uint64_t xmin;
  uint64_t xmax;
  uint16_t flags;
};

struct VarlenHeader {
  uint32_t length;
  const std::byte *data;
};

struct Row {
  std::vector<Value> values;
};

} // namespace record

/**
 * NOTE(andrew):
 * The record format for each row consists of the following:
 * - RowHeader
 * - NULL bitmap (if HASNULL flag is set)
 * - Fixed-width bytes for each table column
 * - For variable-length fields:
 *   1) VarlenHeader specifying length of field
 *   2) byte pointer
 *
 */

class RecordManager {
private:
  std::vector<record::Flag> extract_flags(uint16_t flags);

  bool is_null(const std::byte *data, uint16_t col_idx);
  void set_bit(std::vector<std::byte> &nullbytes, size_t idx);

  record::RowHeader build_row_header(record::Row &row, schema::Table &table);
  void insert_value(std::vector<std::byte> &buffer, record::Value value);

public:
  RecordManager();
  ~RecordManager();
  record::Row deserialize(std::vector<std::byte> &bytes, schema::Table &table);
  std::vector<std::byte> serialize(record::Row &row, schema::Table &table);
};
