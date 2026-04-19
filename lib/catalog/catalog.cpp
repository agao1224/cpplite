#include <cassert>
#include <cstddef>

#include "btree/btree.h"
#include "catalog/catalog.h"

static PageNumber ensure_schema_page(Pager *pager) {
  if (pager->get_num_pages() < CPPLITE_SCHEMA_PGNO) {
    pager->create_page(PAGER_NODE_PAGE);
  }
  return CPPLITE_SCHEMA_PGNO;
}

void CatalogManager::initialize_tables() {
  if (schema_cursor_.is_empty())
    return;

  schema_cursor_.move_to_first();
  do {
    std::vector<std::byte> table_bytes = schema_cursor_.current_value();
    Table table = deserialize_table(table_bytes);
    tables_[table.name] = table;
    table_dependencies_[table.tbl_name].push_back(table.name);
  } while (!schema_cursor_.next());
}

CatalogManager::CatalogManager(Pager *pager)
    : schema_cursor_(pager, ensure_schema_page(pager)) {
  assert(pager != nullptr);
  fpm_ = FirstPageManager(pager->db_file_ptr_);
  next_oid_ = fpm_.next_oid_;
  initialize_tables();
}

void CatalogManager::create_table(Table table) {
  std::vector<std::byte> payload = serialize_table(table);
  schema_cursor_.insert(next_oid_, payload);
  next_oid_++;
  fpm_.set_next_oid(next_oid_);
  tables_[table.name] = table;
  table_dependencies_[table.tbl_name].push_back(table.name);
}

void CatalogManager::drop_table(std::string name) {
  if (tables_.find(name) == tables_.end())
    return;

  Table table = tables_[name];
  schema_cursor_.move_to_key(table.oid);
  schema_cursor_.remove();
  if (table_dependencies_.find(table.name) == table_dependencies_.end())
    return;

  std::vector<std::string> dependencies = table_dependencies_[table.name];
  for (auto dependent_table : dependencies)
    drop_table(dependent_table);
  return;
}
