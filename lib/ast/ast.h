
#pragma once

#include "parser/SQLiteParserBaseVisitor.h"
#include "shared.h"

namespace ast {

struct Stmt {
  virtual StmtType type() const = 0;
  virtual ~Stmt() = default;
};

struct ColumnDef {
  std::string name;
  std::string datatype_str;
  bool is_primary_key;
  bool is_nullable;
};

struct CreateTableNode : Stmt {
  std::string name;
  std::vector<ColumnDef> columns;
  StmtType type() const override { return StmtType::CREATE_TABLE; }
};

struct DropTableNode : Stmt {
  std::string name;
  StmtType type() const override { return StmtType::DROP_TABLE; }
};

} // namespace ast

class ASTBuilder : SQLiteParserBaseVisitor {
private:
  std::any visitCreate_table_stmt(SQLiteParser::Create_table_stmtContext *ctx);
  std::any visitDrop_stmt(SQLiteParser::Drop_stmtContext *ctx);
};
