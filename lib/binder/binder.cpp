
#include "binder/binder.h"
#include <algorithm>
#include <memory>
#include <stdexcept>

static const std::map<std::string, schema::DataType> TYPE_MAP = {
    {"INTEGER", schema::DataType::INTEGER},
    {"INT", schema::DataType::INTEGER},
    {"TEXT", schema::DataType::TEXT},
    {"BOOLEAN", schema::DataType::BOOLEAN},
    {"BOOL", schema::DataType::BOOLEAN},
};

Binder::Binder(CatalogManager *catalog) : catalog_(catalog) {}

std::unique_ptr<binder::Stmt> Binder::bind(ast::Stmt *stmt) {
  switch (stmt->type()) {
  case StmtType::CREATE_TABLE:
    return bind_create_table(static_cast<ast::CreateTableNode *>(stmt));
  case StmtType::DROP_TABLE:
    return bind_drop_table(static_cast<ast::DropTableNode *>(stmt));
  }
}

std::unique_ptr<binder::Stmt>
Binder::bind_create_table(ast::CreateTableNode *stmt) {
  assert(stmt != nullptr);
  std::unique_ptr<binder::CreateTableStmt> create_tbl_stmt =
      std::make_unique<binder::CreateTableStmt>();

  create_tbl_stmt->name = stmt->name;
  for (auto col_def : stmt->columns) {
    schema::Column col_schema;
    col_schema.name = col_def.name;
    col_schema.is_primary_key = col_def.is_primary_key;
    col_schema.is_nullable = col_def.is_nullable;

    std::string type_upper = col_def.datatype_str;
    std::transform(type_upper.begin(), type_upper.end(), type_upper.begin(),
                   ::toupper);
    auto it = TYPE_MAP.find(type_upper);
    if (it == TYPE_MAP.end())
      throw std::runtime_error("[Binder:create_table]: Unknown type " +
                               col_def.datatype_str);
    col_schema.type = it->second;

    create_tbl_stmt->columns.push_back(col_schema);
  }

  return create_tbl_stmt;
}

std::unique_ptr<binder::Stmt>
Binder::bind_drop_table(ast::DropTableNode *stmt) {
  assert(stmt != nullptr);
  std::unique_ptr<binder::DropTableStmt> drop_tbl_stmt =
      std::make_unique<binder::DropTableStmt>();

  drop_tbl_stmt->name = stmt->name;
  return drop_tbl_stmt;
}
