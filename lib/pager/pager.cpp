#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <cstddef>
#include <cassert>

#include "pager.h"


void BasePageManager::set_data(std::vector<std::byte> buffer, size_t num_bytes) {
  if (db_file_ptr_ == nullptr)
    throw std::runtime_error("[BasePageManager]: db_file_ptr_ is NULL");

  OsFile db_file = *db_file_ptr_;
  size_t data_offset;

  if (page_type_ == PAGER_FIRST_PAGE) {
    data_offset = sizeof(PagerFirstPageHeader_t);
  } else {
    assert(page_type_ == PAGER_FREE_PAGE ||
           page_type_ == PAGER_NODE_PAGE || 
           page_type_ == PAGER_OVERFLOW_PAGE);
    data_offset = sizeof(PagerNodePageHeader_t);
  }

  bool write_ok = db_file.os_write(buffer, num_bytes);
  if (!write_ok) {
    char errmsg[255];
    const char* page_manager = (page_type_ == PAGER_FIRST_PAGE) ? "FirstPageManager" : "NodePageManager";
    snprintf(errmsg, 255, "[%s]: Failed to write data", page_manager);
    throw std::runtime_error(errmsg);
  }
}

FirstPageManager::FirstPageManager(std::unique_ptr<OsFile> db_file_ptr) {
  std::vector<std::byte> page_content;

  if (db_file_ptr == nullptr)
    throw std::runtime_error("[FirstPageManager]: db_file_ptr is NULL");

  OsFile db_file = *db_file_ptr;
  ssize_t bytes_read = db_file.os_read(page_content, PAGE_SIZE);
  if (bytes_read == -1)
    throw std::runtime_error("[FirstPageManager]: Failed to read page");

  PagerFirstPageHeader_t first_page_header;
  std::memcpy(&first_page_header, page_content.data(), sizeof(PagerFirstPageHeader_t));

  db_file_ptr_ = std::move(db_file_ptr);
  checksum_ = first_page_header.checksum;
  int32_t data_bytes = (static_cast<int32_t>(sizeof(page_content))
   - static_cast<int32_t>(sizeof(first_page_header)));

  if (checksum_ == CHECKSUM && data_bytes >= 0) {
    page_type_ = first_page_header.page_type;
    num_pages_ = first_page_header.num_pages;

    std::memcpy(data_.data(),
      page_content.data() + sizeof(PagerFirstPageHeader_t),
      static_cast<size_t>(data_bytes)
    );
  } else {
    // NOTE(andrew): If checksum invalid or malformed first page header,
    // we reinitialize header and write to disk
    first_page_header.checksum = checksum_ = CHECKSUM;
    first_page_header.page_type = page_type_ = PAGER_FIRST_PAGE;
    first_page_header.num_pages = num_pages_ = 1;

    std::byte* first_page_header_ptr = reinterpret_cast<std::byte*>(&first_page_header);
    std::vector<std::byte> buffer(first_page_header_ptr, first_page_header_ptr + sizeof(first_page_header));
    db_file.os_write(buffer, sizeof(buffer));
  }
}

FirstPageManager::~FirstPageManager() = default;

void FirstPageManager::set_num_pages(uint64_t new_num_pages) {
  num_pages_ = new_num_pages;
  
  if (db_file_ptr_ == nullptr)
    throw std::runtime_error("[FirstPageManager]: db_file_ptr_ is NULL");
  
  OsFile db_file = *db_file_ptr_;
  off_t num_pages_offset = offsetof(PagerFirstPageHeader_t, num_pages);
  std::vector<std::byte> buffer(sizeof(uint64_t));

  std::memcpy(buffer.data(), &new_num_pages, sizeof(uint64_t));

  bool seek_ok = db_file.os_seek(num_pages_offset);
  if (!seek_ok)
    throw std::runtime_error("[FirstPageManager]: Failed to seek set_num_pages");
  bool write_ok = db_file.os_write(buffer, sizeof(uint64_t));
  if (!write_ok)
    throw std::runtime_error("FirstPageManager]: Failed to write set_num_pages");
}

NodePageManager::NodePageManager(PageNumber page_num, std::unique_ptr<OsFile> db_file_ptr) {
  std::vector<std::byte> page_content;

  if (db_file_ptr == nullptr)
    throw std::runtime_error("[NodePageManager]: db_file_ptr is NULL");

  OsFile db_file = *db_file_ptr;
  ssize_t bytes_read = db_file.os_read(page_content, PAGE_SIZE);
  if (bytes_read == -1)
    throw std::runtime_error("[NodePageManager]: Failed to read page");

  PagerNodePageHeader_t node_page_header;
  std::memcpy(&node_page_header, page_content.data(), sizeof(PagerNodePageHeader_t));

  db_file_ptr_ = std::move(db_file_ptr);
  checksum_ = node_page_header.checksum;
  int32_t data_bytes = (static_cast<int32_t>(sizeof(page_content))
    - static_cast<int32_t>(sizeof(node_page_header)));

  if (checksum_ == CHECKSUM && data_bytes >= 0) {
    page_type_ = node_page_header.page_type;
    std::memcpy(data_.data(),
                page_content.data() + sizeof(PagerNodePageHeader_t),
                static_cast<size_t>(data_bytes));
  } else {
    node_page_header.checksum = checksum_ = CHECKSUM;
    node_page_header.page_type = page_type_ = PAGER_NODE_PAGE;

    std::byte* node_page_header_ptr = reinterpret_cast<std::byte*>(&node_page_header);
    std::vector<std::byte> buffer(node_page_header_ptr, node_page_header_ptr + sizeof(node_page_header));
    db_file.os_write(buffer, sizeof(buffer));
  }
}

NodePageManager::~NodePageManager() = default;
