#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <cstddef>
#include <cassert>
#include <deque>

#include "pager.h"

void BasePageManager::set_data(std::vector<std::byte> buffer, size_t num_bytes, PageNumber page_num) {
  assert(db_file_ptr_ != nullptr);

  OsFile db_file = *db_file_ptr_;
  db_file.os_open();
  size_t data_offset;

  if (page_num == 1) {
    assert(page_type_ == PAGER_FIRST_PAGE);
    data_offset = sizeof(PagerFirstPageHeader_t);
  } else {
    assert(page_type_ != PAGER_FIRST_PAGE);
    data_offset = (page_num-1) * PAGE_SIZE + sizeof(PagerNodePageHeader_t);
  }

  char errmsg[255];
  const char* page_manager = (page_type_ == PAGER_FIRST_PAGE) ? "FirstPageManager" : "NodePageManager";

  bool seek_ok = db_file.os_seek(static_cast<off_t>(data_offset));
  if (!seek_ok) {
    snprintf(errmsg, 255, "[%s]: Failed to seek data", page_manager);
    throw std::runtime_error(errmsg);
  }
  bool write_ok = db_file.os_write(buffer, num_bytes);
  if (!write_ok) {
    snprintf(errmsg, 255, "[%s]: Failed to write data", page_manager);
    throw std::runtime_error(errmsg);
  }
}

FirstPageManager::FirstPageManager(std::shared_ptr<OsFile> db_file_ptr) {
  assert(db_file_ptr_ != nullptr);

  std::vector<std::byte> page_content(PAGE_SIZE);

  OsFile db_file = *db_file_ptr;
  db_file.os_open();
  ssize_t bytes_read = db_file.os_read(page_content, PAGE_SIZE);
  db_file.os_close();
  if (bytes_read == -1)
    throw std::runtime_error("[FirstPageManager]: Failed to read page");

  PagerFirstPageHeader_t first_page_header(page_content);

  db_file_ptr_ = db_file_ptr;
  checksum_ = first_page_header.checksum;
  int32_t num_payload_bytes = (static_cast<int32_t>(PAGE_SIZE)
   - static_cast<int32_t>(sizeof(first_page_header)));

  if (checksum_ == CHECKSUM && num_payload_bytes >= 0) {
    page_type_ = first_page_header.page_type;
    num_pages_ = first_page_header.num_pages;
    free_page_head_ = first_page_header.free_page_head;
    pgno_ = 1;

    data_.resize(num_payload_bytes);
    std::memcpy(data_.data(),
      page_content.data() + sizeof(PagerFirstPageHeader_t),
      static_cast<size_t>(num_payload_bytes)
    );
  } else {
    throw std::runtime_error("[FirstPageManager]: Invalid checksum");
  }
}

FirstPageManager::FirstPageManager() = default;
FirstPageManager::~FirstPageManager() = default;

void FirstPageManager::set_num_pages(uint64_t new_num_pages) {
  assert (db_file_ptr_ != nullptr);
  num_pages_ = new_num_pages;
  
  OsFile db_file = *db_file_ptr_;
  db_file.os_open();
  off_t num_pages_offset = offsetof(PagerFirstPageHeader_t, num_pages);
  std::vector<std::byte> buffer(sizeof(uint64_t));

  std::memcpy(buffer.data(), &new_num_pages, sizeof(uint64_t));

  bool seek_ok = db_file.os_seek(num_pages_offset);
  if (!seek_ok)
    throw std::runtime_error("[FirstPageManager]: Failed to seek set_num_pages");
  bool write_ok = db_file.os_write(buffer, sizeof(uint64_t));
  if (!write_ok)
    throw std::runtime_error("[FirstPageManager]: Failed to write set_num_pages");
  db_file.os_close();
}

void FirstPageManager::set_free_page_head(PageNumber pgno) {
  assert(db_file_ptr_ != nullptr);

  free_page_head_ = pgno;

  OsFile db_file = *db_file_ptr_;
  db_file.os_open();
  off_t free_page_head_offset = offsetof(PagerFirstPageHeader_t, free_page_head);
  std::vector<std::byte> buffer(sizeof(PageNumber));

  std::memcpy(buffer.data(), &pgno, sizeof(PageNumber));

  bool seek_ok = db_file.os_seek(free_page_head_offset);
  if (!seek_ok)
    throw std::runtime_error("[FirstPageManager]: Failed to seek set_free_page_head");
  bool write_ok = db_file.os_write(buffer, sizeof(PageNumber));
  if (!write_ok)
    throw std::runtime_error("[FirstPageManager]: Failed to write set_free_page_head");
  db_file.os_close();
}

PageNumber FirstPageManager::get_free_page_head() {
  return free_page_head_; 
}

PageNumber FirstPageManager::create_free_page() {
  assert(db_file_ptr_ != nullptr);

  num_pages_ += 1;
  PagerFreePageHeader_t free_page_header = PagerFreePageHeader(
    CHECKSUM,
    PAGER_FREE_PAGE,
    free_page_head_
  );
  OsFile db_file = *db_file_ptr_;
  db_file.os_open();
  db_file.os_append(free_page_header.to_bytes(), PAGE_SIZE);
  db_file.os_close();
  return num_pages_;
}

NodePageManager::NodePageManager(PageNumber pgno, std::shared_ptr<OsFile> db_file_ptr) {
  assert(db_file_ptr_ != nullptr);

  std::vector<std::byte> page_content(PAGE_SIZE);

  OsFile db_file = *db_file_ptr;
  db_file.os_open();

  // NOTE(andrew): consider creating a universal helper function that finds the exact
  // offset of specific properties like pages, or even fields within pages
  // since this single operation is being done a lot
  bool seek_ok = db_file.os_seek((pgno-1)*PAGE_SIZE);
  if (!seek_ok)
    throw std::runtime_error("[NodePageManager]: Failed to seek");

  ssize_t bytes_read = db_file.os_read(page_content, PAGE_SIZE);
  db_file.os_close();
  if (bytes_read == -1)
    throw std::runtime_error("[NodePageManager]: Failed to read page");

  PagerNodePageHeader_t node_page_header(page_content);

  db_file_ptr_ = db_file_ptr;
  checksum_ = node_page_header.checksum;
  int32_t num_payload_bytes = (static_cast<int32_t>(bytes_read)
    - static_cast<int32_t>(sizeof(node_page_header)));

  if (checksum_ == CHECKSUM && num_payload_bytes >= 0) {
    assert(node_page_header.page_type == PAGER_NODE_PAGE);
    page_type_ = node_page_header.page_type;
    free_start_ = node_page_header.free_start;
    free_end_ = node_page_header.free_end;
    total_bytes_free_ = node_page_header.total_bytes_free;
    pgno_ = pgno;

    data_.resize(num_payload_bytes);

    size_t num_cell_bytes = static_cast<size_t>(free_start_ - sizeof(PagerNodePageHeader_t));

    cells_.resize(num_cell_bytes / sizeof(PagerCell_t));
    std::memcpy(
      cells_.data(),
      page_content.data() + sizeof(PagerNodePageHeader_t),
      num_cell_bytes
    );

    std::memcpy(data_.data(),
                page_content.data() + free_start_,
                static_cast<size_t>(PAGE_SIZE - free_start_));
  } else { 
    throw std::runtime_error("[NodePageManager]: Invalid checksum");
  }
}

NodePageManager::~NodePageManager() = default;

PageNumber NodePageManager::_create_overflow_pages(std::vector<std::byte> payload) {
  assert(db_file_ptr_ != nullptr);

  PageNumber first_overflow_page = NULL_PAGE;

  std::deque<std::vector<std::byte>> page_chunks;
  size_t chunk_size = PAGE_SIZE - sizeof(PagerOverflowPageHeader_t);
  for (size_t i = 0; i < payload.size(); i += chunk_size) {
    page_chunks.push_back(
      std::vector<std::byte>(
        payload.begin() + i,
        std::min(payload.begin() + i + chunk_size, payload.end())
      )
    );
  }

  FirstPageManager fpm(db_file_ptr_);

  PageNumber free_head = fpm.get_free_page_head();

  PageNumber free_page_ptr = free_head;
  PageNumber prev_page_ptr = NULL_PAGE;

  OsFile db_file = *db_file_ptr_;

  std::vector<std::byte> chunk;
  while (free_page_ptr != NULL_PAGE && !page_chunks.empty()) {
    chunk = page_chunks.front();
    page_chunks.pop_front();

    PagerOverflowPageHeader_t overflow_header(
      CHECKSUM,
      PAGER_OVERFLOW_PAGE,
      NULL_PAGE
    );

    prev_page_ptr = free_page_ptr;
    FreePageManager tmp(free_page_ptr, db_file_ptr_);
    free_page_ptr = tmp.get_next_free_page();
    overflow_header.next_overflow_page = free_page_ptr;
    fpm.set_free_page_head(free_page_ptr);

    std::vector<std::byte> raw_overflow_page = overflow_header.to_bytes();
    raw_overflow_page.insert(raw_overflow_page.begin(), chunk.begin(), chunk.end());

    assert(raw_overflow_page.size() <= PAGE_SIZE);

    if (first_overflow_page == NULL_PAGE)
      first_overflow_page = prev_page_ptr;

    db_file.os_open();

    bool seek_ok = db_file.os_seek((prev_page_ptr-1)*PAGE_SIZE);
    if (!seek_ok)
      throw std::runtime_error("[NodePageManager:_create_overflow_pages]: Failed to seek free page");
    bool write_ok = db_file.os_write(raw_overflow_page, PAGE_SIZE);
    if (!write_ok)
      throw std::runtime_error("[NodePageManager:_create_overflow_pages]: Failed to overwrite free page");

    db_file.os_close();
  }

  std::vector<std::pair<PageNumber, PagerOverflowPageHeader_t>> overflow_headers;
  for (size_t i = 0; i < page_chunks.size(); i++) {
    std::vector<std::byte> payload = page_chunks[i];
    PageNumber overflow_pgno = fpm.create_free_page();
    PagerOverflowPageHeader_t overflow_header(
      CHECKSUM,
      PAGER_OVERFLOW_PAGE,
      NULL_PAGE
    );
    if (i > 0) {
      overflow_headers[i-1].second.next_overflow_page = overflow_pgno;
      PageNumber pgno_to_write = overflow_headers[i-1].first;
      PagerOverflowPageHeader_t header_to_write = overflow_headers[i-1].second;

      db_file.os_open();
      bool seek_ok = db_file.os_seek((pgno_to_write-1)*PAGE_SIZE);
      if (!seek_ok)
        throw std::runtime_error("[NodePageManager:_create_overflow_pages]: Failed to seek free page");
      bool write_ok = db_file.os_write(header_to_write.to_bytes(), PAGE_SIZE);
      if (!write_ok)
        throw std::runtime_error("[NodePageManager:_create_overflow_pages]: Failed to overwrite free page");
      db_file.os_close();
    }
    overflow_headers.push_back(std::make_pair(overflow_pgno, overflow_header));
  }

  PageNumber last_pgno = overflow_headers[overflow_headers.size()-1].first;
  PagerOverflowPageHeader_t last_header = overflow_headers[overflow_headers.size()-1].second;

  db_file.os_open();
  bool seek_ok = db_file.os_seek((last_pgno-1)*PAGE_SIZE);
  if (!seek_ok)
    throw std::runtime_error("[NodePageManager:_create_overflow_pages]: Failed to seek free page");
  bool write_ok = db_file.os_write(last_header.to_bytes(), PAGE_SIZE);
  if (!write_ok)
    throw std::runtime_error("[NodePageManager:_create_overflow_pages]: Failed to overwrite free page");
  db_file.os_close();
  
  fpm.set_free_page_head(NULL_PAGE);
  return first_overflow_page;
}

bool NodePageManager::insert_cell(uint32_t key, std::vector<std::byte> cell_data) {
  assert(free_end_ - free_start_ == total_bytes_free_);
  assert(db_file_ptr_ != nullptr);

  if (sizeof(PagerCell_t) > total_bytes_free_) {
    // NOTE(andrew): Not enough space to write in pager cell,
    // the caller should handle creating another node page to
    // fit this in
    return false;
  }

  assert(sizeof(PagerCell_t) <= total_bytes_free_);
  PagerCell_t pager_cell;
  pager_cell.size = cell_data.size();
  pager_cell.key = key;
  pager_cell.overflow_page = NULL_PAGE;

  uint16_t free_bytes = total_bytes_free_ - sizeof(PagerCell_t);

  OsFile db_file = *db_file_ptr_;
  bool seek_ok, write_ok;

  if (free_bytes < cell_data.size()) {
    // NOTE(andrew): if we cannot fit payload into the remaining free space,
    // offload the entire payload to overflow pages
    pager_cell.overflow_page = _create_overflow_pages(cell_data);
  } else {
    db_file.os_open();
    free_end_ -= cell_data.size();

    pager_cell.offset = free_end_;

    seek_ok = db_file.os_seek((pgno_-1)*PAGE_SIZE + free_end_);
    if (!seek_ok)
      throw std::runtime_error("[NodePageManager:insert_cell]: Failed to seek to write cell_data payload");
    write_ok = db_file.os_write(cell_data, cell_data.size());
    if (!write_ok)
      throw std::runtime_error("[NodePageManager:insert_cell]: Failed to write cell_data payload");

    db_file.os_close();
  }

  db_file.os_open();
  seek_ok = db_file.os_seek((pgno_-1)*PAGE_SIZE + free_start_);
  if (!seek_ok)
    throw std::runtime_error("[NodePageManager:insert_cell]: Failed to seek to write cell header");
  write_ok = db_file.os_write(pager_cell.to_bytes(), sizeof(PagerCell_t));
  if (!write_ok)
    throw std::runtime_error("[NodePageManager:insert_cell]: Failed to write cell header");

  free_start_ += sizeof(PagerCell_t);
  total_bytes_free_ = free_end_ - free_start_;
  PagerNodePageHeader_t node_page_header(
    CHECKSUM,
    PAGER_NODE_PAGE,
    free_start_,
    free_end_,
    total_bytes_free_
  );

  assert(free_end_ - free_start_ == total_bytes_free_);

  seek_ok = db_file.os_seek((pgno_-1)*PAGE_SIZE);
  if (!seek_ok)
    throw std::runtime_error("[NodePageManager:insert_cell]: Failed to seek to write new node page header");
  write_ok = db_file.os_write(node_page_header.to_bytes(), sizeof(PagerNodePageHeader_t));
  if (!write_ok)
    throw std::runtime_error("[NodePageManager:insert_cell]: Failed to write new node page header");
  db_file.os_close();

  return true;
}

std::optional<std::vector<std::byte>> NodePageManager::find_cell(uint32_t key) {
  assert(db_file_ptr_ != nullptr);

  std::optional<PagerCell_t> cell = std::nullopt;
  for (size_t i = 0; i < cells_.size(); i++) {
    if (cells_[i].key == key) {
      cell = cells_[i];
      break;
    }
  }

  if (!cell.has_value())
    return std::nullopt;

  uint16_t num_payload_bytes = cell->size;
  std::vector<std::byte> payload;
  payload.reserve(num_payload_bytes);

  PageNumber it = cell->overflow_page;

  OsFile db_file = *db_file_ptr_;
  if (it != NULL_PAGE) {
    db_file.os_open();
    off_t payload_offset = (pgno_ - 1) * PAGE_SIZE + cell->offset;

    bool seek_ok = db_file.os_seek(payload_offset);
    if (!seek_ok)
      throw std::runtime_error("[NodePageManager]: Failed to seek to read cell payload");
    bool read_ok = db_file.os_read(payload, num_payload_bytes);
    if (!read_ok)
      throw std::runtime_error("[NodePageManager]: Failed to read cell payload");

    db_file.os_close();
    return payload;
  }

  OverflowPageManager page_manager(it, db_file_ptr_);
  std::vector<std::byte> overflow_content;

  while (it != NULL_PAGE) {
    overflow_content = page_manager.get_overflow_content();
    payload.insert(payload.end(), overflow_content.begin(), overflow_content.end());
    it = page_manager.get_next_overflow_page();
  }

  assert(payload.size() == num_payload_bytes);
  return payload;
}

size_t NodePageManager::get_free_space() {
  return total_bytes_free_;
}

FreePageManager::FreePageManager(PageNumber pgno, std::shared_ptr<OsFile> db_file_ptr) {
  assert(db_file_ptr_ != nullptr);

  db_file_ptr_ = db_file_ptr;

  std::vector<std::byte> page_content(PAGE_SIZE);
  OsFile db_file = *db_file_ptr;
  db_file.os_open();
  bool seek_ok = db_file.os_seek((pgno-1) * PAGE_SIZE);
  if (!seek_ok)
    throw std::runtime_error("[FreePageManager]: Failed to seek");

  ssize_t bytes_read = db_file.os_read(page_content, PAGE_SIZE);
  db_file.os_close();
  if (bytes_read == -1)
    throw std::runtime_error("[FreePageManager]: Failed to read page");

  PagerFreePageHeader_t free_page_header(page_content);
  checksum_ = free_page_header.checksum;
  int32_t num_payload_bytes = (static_cast<int32_t>(bytes_read)
   - static_cast<int32_t>(sizeof(free_page_header)));

  if (checksum_ == CHECKSUM && num_payload_bytes >= 0) {
    assert(free_page_header.page_type == PAGER_FREE_PAGE);
    page_type_ = free_page_header.page_type;
    next_free_page_ = free_page_header.next_free_page;
    pgno_ = pgno;
  } else {
    throw std::runtime_error("[FreePageManager]: Invalid checksum");
  }
}

FreePageManager::~FreePageManager() = default;

PageNumber FreePageManager::get_next_free_page() {
  return next_free_page_;
}

void FreePageManager::set_next_free_page(PageNumber pgno) {
  assert(db_file_ptr_ != nullptr);
  next_free_page_ = pgno;

  off_t next_free_page_offset = offsetof(PagerFreePageHeader_t, next_free_page);
  std::vector<std::byte> buffer(sizeof(PageNumber));

  std::memcpy(buffer.data(), &pgno, sizeof(PageNumber));
  OsFile db_file = *db_file_ptr_;
  db_file.os_open();
  bool seek_ok = db_file.os_seek((pgno-1)*PAGE_SIZE + next_free_page_offset);
  if (!seek_ok)
    throw std::runtime_error("[FreePageManager]: Failed to seek set_next_free_page");
  bool write_ok = db_file.os_write(buffer, sizeof(PageNumber));
  if (!write_ok)
    throw std::runtime_error("[FreePageManager]: Failed to write set_next_free_page");
  db_file.os_close();
}

