#include <string>
#include <vector>
#include "shared.h"

#pragma once

enum class DataType {
  INT,
  TEXT,
  BOOLEAN
};

struct Column {
  std::string name;
  DataType type;
  uint32_t length;
  bool is_nullabe;
  bool is_primary_key;
};

struct Table {
  std::string name;
  PageNumber root_pgno;
  std::vector<Column> columns;
};
