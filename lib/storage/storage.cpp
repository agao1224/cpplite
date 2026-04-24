#include "storage/storage.h"
#include "encoding.h"
#include "vfs/localfs.h"

#include <cassert>
#include <cstring>
#include <fcntl.h>
#include <string>
#include <vector>

void StorageEngine::init_control() {
  assert(vfs_ != nullptr);

  if (!vfs_->exists(CONTROL_FILE)) {
    auto control_file = vfs_->open(CONTROL_FILE, O_RDWR | O_CREAT);
    std::vector<std::byte> buffer;
    encoding::append_uint32(buffer, CHECKSUM);
    encoding::append_uint64(buffer, DEFAULT_CATALOG_TABLE);
    encoding::append_uint64(buffer, DEFAULT_CATALOG_PGNO);
    control_file->write(&buffer, buffer.size(), 0);
    return;
  }

  auto control_file = vfs_->open(CONTROL_FILE, O_RDWR);
  std::vector<std::byte> buffer(sizeof(uint32_t) + sizeof(uint64_t) * 2);
  control_file->read(buffer, buffer.size(), 0);

  size_t offset = 0;
  uint32_t control_checksum = encoding::read_uint32(buffer, offset);
  if (control_checksum == CHECKSUM) {
    TableID catalog_tbl_id = encoding::read_uint64(buffer, offset);
    PageNumber catalog_pgno = encoding::read_uint64(buffer, offset);
    control_.catalog_key = PageKey(catalog_tbl_id, catalog_pgno);
  }
  return;
}

void StorageEngine::init_table_metadata() {
  assert(vfs_ != nullptr);

  std::vector<std::string> paths = vfs_->ls("");
  for (auto path : paths) {
    if (!vfs_->is_dir(path))
      continue;

    std::vector<std::string> segments = vfs_->ls(path);
    assert(segments.size() == 1);

    uint64_t last_segmt_filesize =
        vfs_->filesize(segments[segments.size() - 1]);
    PageNumber total_pages =
        (static_cast<uint64_t>(segments.size() - 1) * config_.segment_size) +
        (last_segmt_filesize / config_.page_size);
    storage::TableMetadata metadata(total_pages - 1, total_pages);
    TableID tbl_id = storage::path_to_tbl_id(path);
    table_metadata_[tbl_id] = metadata;
  }
}

void StorageEngine::write_table_metadata() {
  assert(vfs_ != nullptr);
  for (auto const &[table_id, metadata] : table_metadata_) {
    const std::string dir = storage::tbl_path(table_id);
    assert(vfs_->exists(dir));

    storage::SegmentID seg_id = storage::pgno_to_segid(DEFAULT_TABLE_METADATA_PGNO, config_.segment_size);
    const std::string path = storage::seg_path(table_id, seg_id);
    assert(vfs_->exists(path));

    auto metadata_segmt = vfs_->open(path, O_RDWR | O_CREAT);
    if (metadata_segmt == nullptr) {
      throw std::runtime_error(
          "[StorageEngine:init_table_metadata]: Failed to open table file");
    }

    PageNumber rel_pgno = storage::pgno_to_rel_pgno(DEFAULT_TABLE_METADATA_PGNO, config_.segment_size);
    uint64_t offset = storage::pgno_to_offset(rel_pgno, config_.page_size);
    std::vector<std::byte> metadata_bytes = metadata.to_bytes();
    metadata_segmt->write(&metadata_bytes, metadata_bytes.size(), offset);
  }
}


PageNumber StorageEngine::get_latest_page(TableID tbl_id) {
  assert(vfs_ != nullptr);
  auto it = table_metadata_.find(tbl_id);
  PageNumber latest_page;
  if (it == table_metadata_.end()) {
    auto first_segmt = vfs_->open(
        storage::seg_path(tbl_id, DEFAULT_TABLE_METADATA_PGNO), O_RDWR | O_CREAT);
    assert(first_segmt != nullptr);
    std::vector<std::byte> buffer;
    first_segmt->read(buffer, config_.page_size, 0);

    size_t offset = 0;
    storage::TableMetadata metadata(buffer, offset);
    assert(offset == buffer.size());
    table_metadata_[tbl_id] = metadata;
    latest_page = metadata.last_pgno;
  } else {
    latest_page = (it->second).last_pgno;
  }
  return latest_page;
}

StorageEngine::StorageEngine(const std::string &basepath) {
  vfs_ = std::make_unique<LocalFS>(basepath);
  init_control();
  init_table_metadata();
  return;
}

void StorageEngine::read_page(PageKey pgkey, std::vector<std::byte> &buffer) {
  assert(vfs_ != nullptr);
  TableID tbl_id = pgkey.first;
  PageNumber pgno = pgkey.second;

  storage::SegmentID seg_id = storage::pgno_to_segid(pgno, config_.segment_size);
  const std::string dir = storage::tbl_path(tbl_id);
  const std::string path = storage::seg_path(tbl_id, seg_id);

  assert(vfs_->exists(dir) && vfs_->exists(path));

  auto segfile = vfs_->open(path, O_RDWR | O_CREAT);
  if (segfile == nullptr) {
    throw std::runtime_error(
        "[StorageEngine:read_page]: Failed to open segment");
  }

  PageNumber latest_page = get_latest_page(tbl_id);
  if (pgno > latest_page) {
    throw std::runtime_error(
        "[StorageEngine:read_page]: Attempted to read past last page");
  }

  PageNumber rel_pgno = storage::pgno_to_rel_pgno(pgno, config_.segment_size);
  uint64_t offset = storage::pgno_to_offset(rel_pgno, config_.page_size);
  segfile->read(buffer, config_.page_size, offset);
  return;
}

void StorageEngine::write_page(PageKey pgkey, std::vector<std::byte> &buffer) {
  assert(vfs_ != nullptr);
  TableID tbl_id = pgkey.first;
  PageNumber pgno = pgkey.second;

  storage::SegmentID seg_id = storage::pgno_to_segid(pgno, config_.segment_size);
  const std::string dir = storage::tbl_path(tbl_id);
  const std::string path = storage::seg_path(tbl_id, seg_id);

  assert(vfs_->exists(dir) && vfs_->exists(path));

  auto segfile = vfs_->open(path, O_RDWR | O_CREAT);
  if (segfile == nullptr) {
    throw std::runtime_error(
        "[StorageEngine:write_page]: Failed to open segment");
  }

  PageNumber latest_page = get_latest_page(tbl_id);
  if (pgno > latest_page) {
    throw std::runtime_error(
        "[StorageEngine:read_page]: Attempted to write past last page");
  }

  PageNumber rel_pgno = storage::pgno_to_rel_pgno(pgno, config_.segment_size);
  uint64_t offset = storage::pgno_to_offset(rel_pgno, config_.page_size);
  segfile->write(&buffer, config_.page_size, offset);
  return;
}

PageKey StorageEngine::allocate_page(TableID tbl_id) {
  assert(vfs_ != nullptr);

  const std::string dir = storage::tbl_path(tbl_id);
  assert(vfs_->exists(dir));

  PageNumber latest_page = get_latest_page(tbl_id);
  latest_page++;

  storage::SegmentID seg_id = storage::pgno_to_segid(latest_page, config_.segment_size);
  auto segmt = vfs_->open(storage::seg_path(tbl_id, seg_id), O_RDWR | O_CREAT);
  PageNumber rel_pgno = storage::pgno_to_rel_pgno(latest_page, config_.segment_size);
  uint64_t offset = storage::pgno_to_offset(rel_pgno, config_.page_size);

  segmt->write(nullptr, config_.page_size, offset);

  table_metadata_[tbl_id].last_pgno++;
  table_metadata_[tbl_id].total_pages++;
  assert(latest_page == table_metadata_[tbl_id].last_pgno);
  return PageKey(tbl_id, latest_page);
}

void StorageEngine::create_table(TableID tbl_id) {
  assert(vfs_ != nullptr);

  const std::string dir = storage::tbl_path(tbl_id);
  if (vfs_->exists(dir)) {
    throw std::runtime_error(
        "[StorageEngine:create_table]: Table already exists " + dir);
  }

  vfs_->mkdir(dir);
  const std::string path = storage::seg_path(tbl_id, 0);
  auto first_segmt = vfs_->open(path, O_RDWR | O_CREAT);
  if (first_segmt == nullptr) {
    throw std::runtime_error(
        "[StorageEngine:create_table]: Failed to create first segment for " +
        dir);
  }

  storage::TableMetadata metadata(DEFAULT_TABLE_METADATA_PGNO,
                                  DEFAULT_TABLE_METADATA_PGNO + 1);
  table_metadata_[tbl_id] = metadata;
  std::vector<std::byte> buffer = metadata.to_bytes();
  first_segmt->write(&buffer, config_.page_size, 0);
  return;
}

void StorageEngine::flush_table(TableID tbl_id) {
  assert(vfs_ != nullptr);
  std::vector<std::string> seg_files = vfs_->ls(storage::tbl_path(tbl_id));
  for (auto segfile : seg_files) {
    auto segmt = vfs_->open(segfile, O_RDWR | O_CREAT);
    segmt->sync();
  }
  return;
}

void StorageEngine::truncate_table(TableID tbl_id) {
  assert(vfs_ != nullptr);
  std::vector<std::string> segfiles = vfs_->ls(storage::tbl_path(tbl_id));
  for (auto segfile : segfiles)
    vfs_->unlink(segfile);

  assert(vfs_->ls(storage::tbl_path(tbl_id)).size() == 0);
  return;
}

void StorageEngine::delete_table(TableID tbl_id) {
  truncate_table(tbl_id);
  const std::string tbl_dir = storage::tbl_path(tbl_id);
  vfs_->rmdir(tbl_dir);
  assert(!vfs_->exists(tbl_dir));
  return;
}

void StorageEngine::shutdown() {
  write_table_metadata();
  for (auto const &[table, _] : table_metadata_)
    flush_table(table);
}
