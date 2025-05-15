#pragma once
#include "utils.h"
#include <string>
#include <memory>

class Parser {
public:
  explicit Parser(const std::string& input);
  std::unique_ptr<ParsedStatement> parse();

  // PARSER UTILS
  std::unique_ptr<DB_Expression> parseExpr(std::vector<std::string> tokens);

private:
  std::string input_;
  size_t position_;

  // UTILITIES
  void skipWhitespace();
  std::string readToken();
  
  // CREATE TABLE
  void parseCreateTable(std::unique_ptr<ParsedStatement>& stmt); 
  std::unique_ptr<ParsedColumnDef> parseColDef(std::vector<std::string> col_tokens);
};
