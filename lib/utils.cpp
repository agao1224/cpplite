#include "utils.h"
#include <string>

bool Utils::compareNocase(const std::string& str1, const std::string& str2) {
  if (str1.length() != str2.length()) {
    return false;
  }
  for (size_t i = 0; i < str1.length(); ++i) {
    if (std::tolower(str1[i]) != std::tolower(str2[i])) {
      return false;
    }
  }
  return true;
}

void Utils::printColDef(std::unique_ptr<ParsedColumnDef>& col_def) {
  std::string col_type;
  switch (col_def->col_type) {
    case DB_DataType::BOOL:
      col_type = "BOOL";
      break;
    case DB_DataType::INTEGER:
      col_type = "INT";
      break;
    case DB_DataType::TEXT:
      col_type = "TEXT";
      break;
    default:
      break;
  }
  std::string col_def_str = std::format(
    "Col name = {}, Col type = {}",
    col_def->col_name,
    col_type
  );
  DEBUG_PRINT(col_def_str);
}