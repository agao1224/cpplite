
#include "binder/binder.h"
#include "catalog/catalog.h"
#include "catalog/schema.h"
#include "executor/ddl.h"
#include "db_test_fixture.h"
#include <gtest/gtest.h>

class DDLExecutorTest : public DbTestFixture {
protected:
  CatalogManager *catalog;
  DDLExecutor *executor;

  void SetUp() override {
    DbTestFixture::SetUp();
    catalog = new CatalogManager(pager.get());
    executor = new DDLExecutor(catalog);
  }

  void TearDown() override {
    delete executor;
    delete catalog;
    DbTestFixture::TearDown();
  }
};

TEST_F(DDLExecutorTest, ExecuteCreateTable) {
  binder::CreateTableStmt stmt;
  stmt.name = "users";
  stmt.columns = {
      {.type = schema::DataType::INTEGER, .name = "id", .is_primary_key = true, .is_nullable = false},
      {.type = schema::DataType::TEXT, .name = "email", .is_primary_key = false, .is_nullable = true},
  };

  executor->execute(&stmt);

  std::optional<schema::Table> table_opt = catalog->get_table("users");
  ASSERT_TRUE(table_opt.has_value());
  schema::Table table = table_opt.value();
  ASSERT_EQ(table.name, "users");
  ASSERT_EQ(table.columns.size(), 2);
  ASSERT_EQ(table.columns[0].type, schema::DataType::INTEGER);
  ASSERT_EQ(table.columns[0].name, "id");
  ASSERT_EQ(table.columns[1].type, schema::DataType::TEXT);
  ASSERT_EQ(table.columns[1].name, "email");
}

TEST_F(DDLExecutorTest, ExecuteDropTable) {
  binder::CreateTableStmt create_stmt;
  create_stmt.name = "users";
  create_stmt.columns = {
      {.type = schema::DataType::INTEGER, .name = "id", .is_primary_key = true, .is_nullable = false},
  };
  executor->execute(&create_stmt);

  binder::DropTableStmt drop_stmt;
  drop_stmt.name = "users";
  executor->execute(&drop_stmt);

  std::optional<schema::Table> table_opt = catalog->get_table("users");
  ASSERT_FALSE(table_opt.has_value());
}
