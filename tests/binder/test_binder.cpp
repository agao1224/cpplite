
#include "binder/binder.h"
#include "catalog/catalog.h"
#include "catalog/schema.h"
#include "db_test_fixture.h"
#include <gtest/gtest.h>

class BinderTest : public DbTestFixture {
protected:
  CatalogManager *catalog;
  Binder *binder;

  void SetUp() override {
    DbTestFixture::SetUp();
    catalog = new CatalogManager(pager.get());
    binder = new Binder(catalog);
  }

  void TearDown() override {
    delete binder;
    delete catalog;
    DbTestFixture::TearDown();
  }
};

TEST_F(BinderTest, BindCreateTable) {
  ast::CreateTableNode stmt;
  stmt.name = "users";
  stmt.columns = {
      {.name = "id", .datatype_str = "INTEGER", .is_primary_key = true, .is_nullable = false},
      {.name = "email", .datatype_str = "TEXT", .is_primary_key = false, .is_nullable = true},
  };

  std::unique_ptr<binder::Stmt> bound = binder->bind(&stmt);
  ASSERT_NE(bound, nullptr);
  ASSERT_EQ(bound->type(), StmtType::CREATE_TABLE);

  auto *create = static_cast<binder::CreateTableStmt *>(bound.get());
  ASSERT_EQ(create->name, "users");
  ASSERT_EQ(create->columns.size(), 2);
  ASSERT_EQ(create->columns[0].type, schema::DataType::INTEGER);
  ASSERT_EQ(create->columns[0].name, "id");
  ASSERT_EQ(create->columns[0].is_primary_key, true);
  ASSERT_EQ(create->columns[1].type, schema::DataType::TEXT);
  ASSERT_EQ(create->columns[1].name, "email");
  ASSERT_EQ(create->columns[1].is_nullable, true);
}

TEST_F(BinderTest, BindDropTable) {
  ast::DropTableNode stmt;
  stmt.name = "users";

  std::unique_ptr<binder::Stmt> bound = binder->bind(&stmt);
  ASSERT_NE(bound, nullptr);
  ASSERT_EQ(bound->type(), StmtType::DROP_TABLE);

  auto *drop = static_cast<binder::DropTableStmt *>(bound.get());
  ASSERT_EQ(drop->name, "users");
}
