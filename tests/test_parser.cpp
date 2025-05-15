#include "gtest/gtest.h"
#include "parser.h"
#include <string>

const std::string tbl_name = "tablename";

TEST(ParserTest, Parser_CreateTable_NoColsFail) {
  std::string in = "CREATE TABLE IF NOT EXISTS tablename";
  Parser parser(in);
  std::unique_ptr<ParsedStatement> stmt = parser.parse();  
  ASSERT_FALSE(stmt->is_valid_stmt);  
}

TEST(ParserTest, Parser_CreateTable_Success) {
  std::string in = std::format("create table {} (col1 INT, col2 TEXT, col3 BOOL)", tbl_name);  
  Parser parser(in);
  std::unique_ptr<ParsedStatement> stmt = parser.parse();
  ASSERT_TRUE(stmt->is_valid_stmt);
  ASSERT_STREQ(stmt->table_name.c_str(), tbl_name.c_str());
}

TEST(ParserTest, Parser_CreateTable_IfNotExistsSuccess) {
  std::string in = std::format("create table if not exists {} (email TEXT)", tbl_name);
  Parser parser(in);
  std::unique_ptr<ParsedStatement> stmt = parser.parse();
  ASSERT_TRUE(stmt->is_valid_stmt);
  ASSERT_STREQ(stmt->table_name.c_str(), tbl_name.c_str());
}