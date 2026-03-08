#include <cstddef>
#include <cstring>
#include <cstddef>
#include <cassert>

#include "free_page.h"


FreePageManager::FreePageManager(PageNumber pgno, std::shared_ptr<OsFile> db_file_ptr) {
  assert(db_file_ptr != nullptr);

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
  bool seek_ok = db_file.os_seek((pgno_-1)*PAGE_SIZE + next_free_page_offset);
  if (!seek_ok)
    throw std::runtime_error("[FreePageManager]: Failed to seek set_next_free_page");
  bool write_ok = db_file.os_write(buffer, sizeof(PageNumber));
  if (!write_ok)
    throw std::runtime_error("[FreePageManager]: Failed to write set_next_free_page");
  db_file.os_close();
}
