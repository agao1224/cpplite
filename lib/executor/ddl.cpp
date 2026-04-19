
#include "executor/ddl.h"
#include "catalog/catalog.h"

DDLExecutor::DDLExecutor(CatalogManager *catalog) : catalog_(catalog) {};

void DDLExecutor::execute(binder::Stmt *stmt) {
  switch (stmt->type()) {
  case StmtType::CREATE_TABLE:
    return execute_create_table(static_cast<binder::CreateTableStmt *>(stmt));
  case StmtType::DROP_TABLE:
    return execute_drop_table(static_cast<binder::DropTableStmt *>(stmt));
  }
}

void DDLExecutor::execute_create_table(binder::CreateTableStmt *stmt) {
  assert(stmt != nullptr);
  assert(stmt->type() == StmtType::CREATE_TABLE);

  schema::Table table;
  table.name = stmt->name;
  table.tbl_name = stmt->name;
  table.type = schema::SchemaType::TABLE;
  table.columns = stmt->columns;

  catalog_->create_table(table);
}

void DDLExecutor::execute_drop_table(binder::DropTableStmt *stmt) {
  assert(stmt != nullptr);
  assert(stmt->type() == StmtType::DROP_TABLE);

  catalog_->drop_table(stmt->name);
}
