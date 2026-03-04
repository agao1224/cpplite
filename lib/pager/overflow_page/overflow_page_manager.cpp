#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <cstddef>
#include <cassert>
#include <deque>

#include "pager.h"


OverflowPageManager::OverflowPageManager(PageNumber pgno, std::shared_ptr<OsFile> db_file_ptr) {
  assert(db_file_ptr_ != nullptr);

  db_file_ptr_ = db_file_ptr;

  std::vector<std::byte> page_content(PAGE_SIZE);
  OsFile db_file = *db_file_ptr;
  bool seek_ok = db_file.os_seek((pgno-1) * PAGE_SIZE);
  if (!seek_ok)
    throw std::runtime_error("[OverflowPageManager]: Failed to seek");
  ssize_t bytes_read = db_file.os_read(page_content, PAGE_SIZE);
  db_file.os_close();
  if (bytes_read == -1)
    throw std::runtime_error("[OverflowPageManager]: Failed to read page");

  PagerOverflowPageHeader_t overflow_page_header(page_content);
  checksum_ = overflow_page_header.checksum;
  int32_t num_payload_bytes = (static_cast<int32_t>(bytes_read)
   - static_cast<int32_t>(sizeof(overflow_page_header)));

  if (checksum_ == CHECKSUM && num_payload_bytes >= 0) {
    assert(overflow_page_header.page_type == PAGER_OVERFLOW_PAGE);
    page_type_ = overflow_page_header.page_type;
    next_overflow_page_ = overflow_page_header.next_overflow_page;
    pgno_ = pgno;
    data_.resize(num_payload_bytes);

    std::memcpy(data_.data(),
            page_content.data() + sizeof(PagerOverflowPageHeader_t),
              static_cast<size_t>(num_payload_bytes));
  } else {
    throw std::runtime_error("[OverflowPageHeader]: Invalid checksum");
  }
}

PageNumber OverflowPageManager::get_next_overflow_page() {
  return next_overflow_page_;
}

void OverflowPageManager::set_next_overflow_page(PageNumber pgno) {
  assert(db_file_ptr_ != nullptr);

  std::vector<std::byte> page_content;
  page_content.reserve(sizeof(PagerOverflowPageHeader_t));

  OsFile db_file = *db_file_ptr_;
  bool seek_ok = db_file.os_seek((pgno_-1) * PAGE_SIZE);
  if (!seek_ok)
    throw std::runtime_error("[OverflowPageManager]: Failed to seek set_next_overflow_page");
  ssize_t bytes_read = db_file.os_read(page_content, sizeof(PagerOverflowPageHeader_t));
  if (bytes_read == -1)
    throw std::runtime_error("[OverflowPageManager]: Failed to read set_next_overflow_page");

  PagerOverflowPageHeader_t page_header(page_content);
  page_header.next_overflow_page = pgno;

  std::memcpy(
    page_content.data(),
    page_header.to_bytes().data(),
    sizeof(PagerOverflowPageHeader_t)
  );

  seek_ok = db_file.os_seek((pgno_-1) * PAGE_SIZE);
  if (!seek_ok)
    throw std::runtime_error("[OverflowPageManager]: Failed to seek to write set_next_overflow_page");
  bool write_ok = db_file.os_write(page_content, sizeof(PagerOverflowPageHeader_t));
  if (!write_ok)
    throw std::runtime_error("[OverflowPageManager]: Failed to write set_next_overflow_page");

  db_file.os_close();
}

std::vector<std::byte> OverflowPageManager::get_overflow_content() {
  assert(db_file_ptr_ != nullptr);

  OsFile db_file = *db_file_ptr_;

  size_t num_overflow_bytes = PAGE_SIZE - sizeof(PagerOverflowPageHeader_t);
  off_t overflow_content_offset = (pgno_-1) * PAGE_SIZE + sizeof(PagerOverflowPageHeader_t);
  std::vector<std::byte> overflow_page_content(num_overflow_bytes);

  bool seek_ok = db_file.os_seek(overflow_content_offset);
  if (!seek_ok)
    throw std::runtime_error("[OverflowPageManager]: Failed to seek to get_overflow_content");

  ssize_t bytes_read = db_file.os_read(overflow_page_content, num_overflow_bytes);
  if (bytes_read != -1)
    throw std::runtime_error("OverflowPageManager]: Failed to read to get_overflow_content");

  db_file.os_close();
  return overflow_page_content;
}

OverflowPageManager::~OverflowPageManager() = default;
