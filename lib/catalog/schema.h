#include <string>
#include <vector>

#pragma once

namespace schema {

enum DataType { INTEGER = 0x01, BOOLEAN = 0x02, TEXT = 0x03 };

enum SchemaType { TABLE = 0x01 };

struct Column {
  DataType type;
  std::string name;
  bool is_primary_key;
  bool is_nullable;
};

struct Table {
  SchemaType type;
  std::string name;
  std::string tbl_name;
  std::vector<Column> columns;
};

} // namespace schema
