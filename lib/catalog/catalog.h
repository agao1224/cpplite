#include <map>

#include "btree/btree.h"
#include "pager/pager.h"
#include "schema.h"

#pragma once

const PageNumber CPPLITE_SCHEMA_PGNO = 2;

class CatalogManager {
  private:
    Pager* pager_;
    BTreeCursor schema_table_cursor_;
    std::map<std::string, Table> tables_;

  public:
    CatalogManager(Pager* pager);
    ~CatalogManager();

    void create_table(Table table);
    void drop_table(Table table);
};