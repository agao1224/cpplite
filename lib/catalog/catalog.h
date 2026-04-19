#include <map>
#include <optional>
#include <string>
#include <vector>

#include "btree/btree.h"
#include "catalog/schema.h"
#include "pager/base_page.h"
#include "pager/pager.h"
#include "shared.h"

#pragma once

const PageNumber CPPLITE_SCHEMA_PGNO = 2;

class CatalogManager {
private:
  Pager *pager_;
  std::map<std::string, schema::Table> tables_;
  std::map<std::string, DefaultPagerKey> table_oids_;
  std::map<std::string, PageNumber> table_root_pages_;
  std::map<std::string, std::vector<std::string>> table_dependencies_;
  BTreeCursor schema_cursor_;
  DefaultPagerKey next_oid_;
  FirstPageManager fpm_;

  void initialize_tables();
  std::vector<std::byte> serialize_table(schema::Table table, PageNumber root_pgno);
  std::pair<schema::Table, PageNumber> deserialize_table(std::vector<std::byte> &bytes);

public:
  CatalogManager(Pager *pager);
  ~CatalogManager();

  void create_table(schema::Table table);
  void drop_table(std::string tbl_name);
  std::optional<schema::Table> get_table(std::string name);
};
