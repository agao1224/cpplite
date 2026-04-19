#include <string>
#include <vector>

#include "pager/base_page.h"

#pragma once

enum DataType { INTEGER = 0x01, BOOLEAN = 0x02, TEXT = 0x03 };

enum SchemaType { TABLE = 0x01 };

struct Column {
  DataType type;
  std::string name;
  bool is_primary_key;
  bool is_nullable;
};

struct Table {
  DefaultPagerKey oid;
  SchemaType type;
  std::string name;
  std::string tbl_name;
  PageNumber root_pgno;
  std::vector<Column> columns;
};
