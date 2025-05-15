#pragma once
#include <iostream>
#include <string>
#include <optional>
#include <algorithm>
#include <cctype>

#ifdef DEBUG
#define DEBUG_PRINT(x) std::cout << "[DEBUG] " << x << std::endl
#else
#define DEBUG_PRINT(x)
#endif

enum DB_DataType {
  INTEGER,
  TEXT,
  BOOL
};

enum DB_ConstraintType {
  PRIMARY_KEY,
  UNIQUE,
  NOT_NULL,
  CHECK
};

enum DB_CmpOp {
  NEQ,
  EQ,
  GT,
  GTE,
  LT,
  LTE,
  LENGTH,
  LITERAL
};

enum Parser_StatementType {
  CREATE_TABLE,
  SELECT
};

struct DB_Expression {
  /*
  For binary op expressions:
  sym1 (OP) sym2
  e.g. sym1 <= sym2

  For unary op expressions:
  OP(sym1)
  e.g. length(sym1)

  */
  DB_CmpOp op;
  std::string literal; 
  std::unique_ptr<DB_Expression> sym1;
  std::unique_ptr<DB_Expression> sym2;
};

struct DB_Constraint {
  DB_ConstraintType type;
  std::string check_expr;
};

struct ParsedColumnDef {
  std::string col_name;
  DB_DataType col_type;
  std::vector<std::unique_ptr<DB_Constraint>> col_constraints;
};

struct ParsedStatement {
  Parser_StatementType type;  // e.g., SELECT, INSERT, etc.

  // Valid flag
  bool is_valid_stmt;

  /* BEGIN: CREATE TABLE */
  std::string table_name;
  bool if_not_exists;

  std::vector<ParsedColumnDef> column_defs;

  /* END: CREATE TABLE */
};

class Utils {
  public:
    static bool compareNocase(const std::string& str1, const std::string& str2);
    static void printColDef(std::unique_ptr<ParsedColumnDef>& col_def);
};
