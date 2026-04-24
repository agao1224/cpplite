#pragma once

#include "encoding.h"
#include "shared.h"
#include "storage/storage_utils.h"
#include "vfs/vfs.h"
#include <cstdint>
#include <map>
#include <string>

static constexpr uint64_t DEFAULT_PAGES_PER_SEGMENT = 131072;
static constexpr uint64_t DEFAULT_PAGE_SIZE = 4096;
static constexpr PageNumber DEFAULT_TABLE_METADATA_PGNO = 0;
static const std::string CONTROL_FILE = "db.control";

static const TableID DEFAULT_CATALOG_TABLE = 0;
static const PageNumber DEFAULT_CATALOG_PGNO = 0;

namespace storage {

struct EngineConfig {
  uint64_t segment_size = DEFAULT_PAGES_PER_SEGMENT;
  uint64_t page_size = DEFAULT_PAGE_SIZE;
};

struct Control {
  uint32_t checksum;
  PageKey catalog_key;
};

struct TableMetadata {
  PageNumber last_pgno;
  PageNumber total_pages;

  const std::vector<std::byte> to_bytes() const {
    std::vector<std::byte> buffer;
    encoding::append_uint64(buffer, last_pgno);
    encoding::append_uint64(buffer, total_pages);
    return buffer;
  }

  TableMetadata(PageNumber last_pgno_, PageNumber total_pages_)
      : last_pgno(last_pgno_), total_pages(total_pages_) {}

  TableMetadata(std::vector<std::byte> &buffer, size_t &offset) {
    last_pgno = static_cast<PageNumber>(encoding::read_uint64(buffer, offset));
    total_pages =
        static_cast<PageNumber>(encoding::read_uint64(buffer, offset));
  }
};

} // namespace storage

class StorageEngine {
private:
  std::unique_ptr<VFS> vfs_;
  storage::EngineConfig config_;
  storage::Control control_;
  std::map<TableID, storage::TableMetadata> table_metadata_;

  PageNumber get_latest_page(TableID tbl_id);

  void init_control();
  void init_table_metadata();
  void write_table_metadata();

public:
  StorageEngine(const std::string &basepath);

  void read_page(PageKey pgkey, std::vector<std::byte> &buffer);
  void write_page(PageKey pgkey, std::vector<std::byte> &buffer);
  PageKey allocate_page(TableID tbl_id);

  void create_table(TableID tbl_id);
  void flush_table(TableID tbl_id);
  void truncate_table(TableID tbl_id);
  void delete_table(TableID tbl_id);
  void shutdown();
};
