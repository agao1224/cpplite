#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <cstddef>
#include <cassert>

#include "first_page.h"
#include "pager/free_page/free_page.h"

FirstPageManager::FirstPageManager(std::shared_ptr<OsFile> db_file_ptr) {
  assert(db_file_ptr != nullptr);

  std::vector<std::byte> page_content(PAGE_SIZE);

  OsFile db_file = *db_file_ptr;
  db_file.os_open();
  ssize_t bytes_read = db_file.os_read(page_content, PAGE_SIZE);
  db_file.os_close();

  PagerFirstPageHeader_t first_page_header(page_content);

  db_file_ptr_ = db_file_ptr;
  checksum_ = first_page_header.checksum;
  int32_t num_payload_bytes = (static_cast<int32_t>(PAGE_SIZE)
   - static_cast<int32_t>(sizeof(first_page_header)));

  if (checksum_ == CHECKSUM && num_payload_bytes >= 0) {
    page_type_ = first_page_header.page_type;
    num_pages_ = first_page_header.num_pages;
    free_page_head_ = first_page_header.free_page_head;
    next_oid_ = first_page_header.next_oid;
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

DefaultPagerKey FirstPageManager::get_next_oid() {
  return next_oid_;
}

void FirstPageManager::set_next_oid(DefaultPagerKey oid) {
  assert(db_file_ptr_ != nullptr);

  next_oid_ = oid;

  OsFile db_file = *db_file_ptr_;
  db_file.os_open();
  off_t next_oid_offset = offsetof(PagerFirstPageHeader_t, next_oid);
  std::vector<std::byte> buffer(sizeof(DefaultPagerKey));
  std::memcpy(buffer.data(), &oid, sizeof(DefaultPagerKey));
  bool seek_ok = db_file.os_seek(next_oid_offset);
  if (!seek_ok)
    throw std::runtime_error("[FirstPageManager]: Failed to seek set_next_oid");
  bool write_ok = db_file.os_write(buffer, sizeof(DefaultPagerKey));
  if (!write_ok)
    throw std::runtime_error("[FirstPageManager]: Failed to write set_next_oid");
  db_file.os_close();
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