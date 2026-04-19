
#pragma once

#include <memory>

#include "ast/ast.h"
#include "catalog/catalog.h"
#include "catalog/schema.h"
#include "shared.h"

namespace binder {

struct Stmt {
  virtual StmtType type() const = 0;
  virtual ~Stmt() = default;
};

struct CreateTableStmt : Stmt {
  std::string name;
  std::vector<schema::Column> columns;
  StmtType type() const override { return StmtType::CREATE_TABLE; }
};

struct DropTableStmt : Stmt {
  std::string name;
  StmtType type() const override { return StmtType::DROP_TABLE; }
};
} // namespace binder

class Binder {
private:
  CatalogManager *catalog_;

  std::unique_ptr<binder::Stmt> bind_create_table(ast::CreateTableNode *stmt);
  std::unique_ptr<binder::Stmt> bind_drop_table(ast::DropTableNode *stmt);

public:
  Binder(CatalogManager *catalog);
  std::unique_ptr<binder::Stmt> bind(ast::Stmt *stmt);
};
