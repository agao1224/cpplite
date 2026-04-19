
#pragma once

#include "binder/binder.h"
#include "catalog/catalog.h"

class DDLExecutor {
private:
  CatalogManager *catalog_;
  void execute_create_table(binder::CreateTableStmt *stmt);
  void execute_drop_table(binder::DropTableStmt *stmt);

public:
  DDLExecutor(CatalogManager *catalog);
  void execute(binder::Stmt *stmt);
};
