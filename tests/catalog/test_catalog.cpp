
#include "catalog/catalog.h"
#include "catalog/schema.h"
#include "db_test_fixture.h"
#include <gtest/gtest.h>

class CatalogTest : public DbTestFixture {};

TEST_F(CatalogTest, CreateTableBasic) {
  CatalogManager catalog(pager.get());

  Table table = {.type = SchemaType::TABLE,
                 .name = "testing",
                 .tbl_name = "testing",
                 .root_pgno = 3,
                 .columns = {
                     {.type = DataType::INTEGER,
                      .name = "id",
                      .is_primary_key = true,
                      .is_nullable = false},
                     {.type = DataType::TEXT,
                      .name = "email",
                      .is_primary_key = false,
                      .is_nullable = true},
                 }};

  catalog.create_table(table);
  std::optional<Table> new_table_opt = catalog.get_table("testing");
  ASSERT_TRUE(new_table_opt.has_value());
  Table new_table = new_table_opt.value();
  ASSERT_EQ(new_table.columns.size(), 2);
  ASSERT_EQ(new_table.name, "testing");
  ASSERT_EQ(new_table.tbl_name, "testing");
  ASSERT_EQ(new_table.type, SchemaType::TABLE);
  ASSERT_EQ(new_table.root_pgno, 3);

  ASSERT_EQ(new_table.columns[0].type, DataType::INTEGER);
  ASSERT_EQ(new_table.columns[0].name, "id");
  ASSERT_EQ(new_table.columns[0].is_primary_key, true);
  ASSERT_EQ(new_table.columns[0].is_nullable, false);

  ASSERT_EQ(new_table.columns[1].type, DataType::TEXT);
  ASSERT_EQ(new_table.columns[1].name, "email");
  ASSERT_EQ(new_table.columns[1].is_primary_key, false);
  ASSERT_EQ(new_table.columns[1].is_nullable, true);
}

TEST_F(CatalogTest, DropTable) {
  CatalogManager catalog(pager.get());

  Table table = {.type = SchemaType::TABLE,
                 .name = "testing",
                 .tbl_name = "testing",
                 .root_pgno = 3,
                 .columns = {
                     {.type = DataType::INTEGER,
                      .name = "id",
                      .is_primary_key = true,
                      .is_nullable = false},
                     {.type = DataType::TEXT,
                      .name = "email",
                      .is_primary_key = false,
                      .is_nullable = true},
                 }};

  catalog.create_table(table);
  catalog.drop_table("testing");

  std::optional<Table> table_opt = catalog.get_table("testing");
  ASSERT_TRUE(!table_opt.has_value());
}

TEST_F(CatalogTest, CreateAndPersistTable) {
  CatalogManager catalog(pager.get());

  Table table = {.type = SchemaType::TABLE,
                 .name = "testing",
                 .tbl_name = "testing",
                 .root_pgno = 3,
                 .columns = {
                     {.type = DataType::INTEGER,
                      .name = "id",
                      .is_primary_key = true,
                      .is_nullable = false},
                     {.type = DataType::TEXT,
                      .name = "email",
                      .is_primary_key = false,
                      .is_nullable = true},
                 }};

  catalog.create_table(table);

  CatalogManager new_catalog(pager.get());

  std::optional<Table> new_table_opt = new_catalog.get_table("testing");
  ASSERT_TRUE(new_table_opt.has_value());
  Table new_table = new_table_opt.value();
  ASSERT_EQ(new_table.columns.size(), 2);
  ASSERT_EQ(new_table.name, "testing");
  ASSERT_EQ(new_table.tbl_name, "testing");
  ASSERT_EQ(new_table.type, SchemaType::TABLE);
  ASSERT_EQ(new_table.root_pgno, 3);

  ASSERT_EQ(new_table.columns[0].type, DataType::INTEGER);
  ASSERT_EQ(new_table.columns[0].name, "id");
  ASSERT_EQ(new_table.columns[0].is_primary_key, true);
  ASSERT_EQ(new_table.columns[0].is_nullable, false);

  ASSERT_EQ(new_table.columns[1].type, DataType::TEXT);
  ASSERT_EQ(new_table.columns[1].name, "email");
  ASSERT_EQ(new_table.columns[1].is_primary_key, false);
  ASSERT_EQ(new_table.columns[1].is_nullable, true);
}
