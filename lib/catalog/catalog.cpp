#include <cassert>
#include <cstddef>
#include <optional>
#include <utility>

#include "btree/btree.h"
#include "catalog/catalog.h"
#include "catalog/schema.h"
#include "encoding.h"

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
    auto [table, root_pgno] = deserialize_table(table_bytes);
    tables_[table.name] = table;
    table_oids_[table.name] = oid;
    table_root_pages_[table.name] = root_pgno;
    table_dependencies_[table.tbl_name].push_back(table.name);
  } while (schema_cursor_.next());
}

CatalogManager::CatalogManager(Pager *pager)
    : pager_(pager), schema_cursor_(pager, ensure_schema_page(pager)) {
  assert(pager != nullptr);
  fpm_ = FirstPageManager(pager->db_file_ptr_);
  next_oid_ = fpm_.next_oid_;
  initialize_tables();
}

CatalogManager::~CatalogManager() {}

void CatalogManager::create_table(schema::Table table) {
  PageNumber root_pgno = pager_->create_page(PAGER_LEAF_PAGE);
  std::vector<std::byte> payload = serialize_table(table, root_pgno);
  schema_cursor_.insert(next_oid_, payload);
  table_oids_[table.name] = next_oid_;
  table_root_pages_[table.name] = root_pgno;
  next_oid_++;
  fpm_.set_next_oid(next_oid_);
  tables_[table.name] = table;
  table_dependencies_[table.tbl_name].push_back(table.name);
}

void CatalogManager::drop_table(std::string name) {
  if (tables_.find(name) == tables_.end())
    return;

  schema::Table table = tables_[name];
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

std::optional<schema::Table> CatalogManager::get_table(std::string name) {
  auto it = tables_.find(name);
  if (it == tables_.end())
    return std::nullopt;
  return it->second;
}

std::vector<std::byte> CatalogManager::serialize_table(schema::Table table,
                                                       PageNumber root_pgno) {
  std::vector<std::byte> serialized;

  serialized.push_back(static_cast<std::byte>(table.type));

  uint16_t name_len = table.name.size();
  encoding::append_uint16(serialized, name_len);
  for (uint16_t i = 0; i < name_len; i++)
    serialized.push_back(static_cast<std::byte>(table.name[i]));

  uint16_t tbl_name_len = table.tbl_name.size();
  encoding::append_uint16(serialized, tbl_name_len);
  for (uint16_t i = 0; i < tbl_name_len; i++)
    serialized.push_back(static_cast<std::byte>(table.tbl_name[i]));

  encoding::append_uint64(serialized, static_cast<uint64_t>(root_pgno));
  uint16_t num_cols = table.columns.size();
  encoding::append_uint16(serialized, num_cols);
  for (uint16_t i = 0; i < num_cols; i++) {
    schema::Column col = table.columns[i];
    serialized.push_back(static_cast<std::byte>(col.type));

    uint16_t col_name_len = col.name.size();
    encoding::append_uint16(serialized, col_name_len);
    for (uint16_t j = 0; j < col_name_len; j++)
      serialized.push_back(static_cast<std::byte>(col.name[j]));

    serialized.push_back(static_cast<std::byte>(col.is_primary_key));
    serialized.push_back(static_cast<std::byte>(col.is_nullable));
  }
  return serialized;
}

std::pair<schema::Table, PageNumber>
CatalogManager::deserialize_table(std::vector<std::byte> &payload) {
  schema::Table table;
  size_t offset = 0;

  table.type = static_cast<schema::SchemaType>(payload[offset]);
  offset++;

  uint16_t name_len = encoding::read_uint16(payload, offset);
  std::string name(reinterpret_cast<const char *>(payload.data() + offset),
                   name_len);
  table.name = name;
  offset += name_len;

  uint16_t tbl_name_len = encoding::read_uint16(payload, offset);
  std::string tbl_name(reinterpret_cast<const char *>(payload.data() + offset),
                       tbl_name_len);
  table.tbl_name = tbl_name;
  offset += tbl_name_len;

  PageNumber root_pgno =
      static_cast<PageNumber>(encoding::read_uint64(payload, offset));

  uint16_t num_cols = encoding::read_uint16(payload, offset);
  for (uint16_t i = 0; i < num_cols; i++) {
    schema::Column col;
    col.type = static_cast<schema::DataType>(payload[offset]);
    offset++;

    uint16_t col_name_len = encoding::read_uint16(payload, offset);
    std::string col_name(
        reinterpret_cast<const char *>(payload.data() + offset), col_name_len);
    col.name = col_name;
    offset += col_name_len;

    col.is_primary_key = static_cast<bool>(payload[offset]);
    offset++;
    col.is_nullable = static_cast<bool>(payload[offset]);
    offset++;

    table.columns.push_back(col);
  }
  assert(payload.size() == offset);
  return {table, root_pgno};
}
