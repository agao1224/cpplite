#include <cassert>
#include <cstddef>
#include <optional>

#include "btree/btree.h"
#include "catalog/catalog.h"

static PageNumber ensure_schema_page(Pager *pager) {
  if (pager->get_num_pages() < CPPLITE_SCHEMA_PGNO) {
    pager->create_page(PAGER_LEAF_PAGE);
  }
  return CPPLITE_SCHEMA_PGNO;
}

void CatalogManager::initialize_tables() {
  if (schema_cursor_.is_empty())
    return;

  schema_cursor_.move_to_first();
  do {
    DefaultPagerKey oid = schema_cursor_.current_key();
    std::vector<std::byte> table_bytes = schema_cursor_.current_value();
    Table table = deserialize_table(table_bytes);
    tables_[table.name] = table;
    table_oids_[table.name] = oid;
    table_dependencies_[table.tbl_name].push_back(table.name);
  } while (schema_cursor_.next());
}

CatalogManager::CatalogManager(Pager *pager)
    : schema_cursor_(pager, ensure_schema_page(pager)) {
  assert(pager != nullptr);
  fpm_ = FirstPageManager(pager->db_file_ptr_);
  next_oid_ = fpm_.next_oid_;
  initialize_tables();
}

CatalogManager::~CatalogManager() {}

void CatalogManager::create_table(Table table) {
  std::vector<std::byte> payload = serialize_table(table);
  schema_cursor_.insert(next_oid_, payload);
  table_oids_[table.name] = next_oid_;
  next_oid_++;
  fpm_.set_next_oid(next_oid_);
  tables_[table.name] = table;
  table_dependencies_[table.tbl_name].push_back(table.name);
}

void CatalogManager::drop_table(std::string name) {
  if (tables_.find(name) == tables_.end())
    return;

  Table table = tables_[name];
  tables_.erase(name);
  schema_cursor_.move_to_key(table_oids_[name]);
  table_oids_.erase(name);
  schema_cursor_.remove();
  if (table_dependencies_.find(table.name) == table_dependencies_.end())
    return;

  std::vector<std::string> dependencies = table_dependencies_[table.name];
  for (auto dependent_table : dependencies)
    drop_table(dependent_table);
  return;
}

std::optional<Table> CatalogManager::get_table(std::string name) {
  auto it = tables_.find(name);
  if (it == tables_.end())
    return std::nullopt;
  return it->second;
}
