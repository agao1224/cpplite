#include <array>
#include <cstddef>
#include <cstring>
#include <memory>
#include <stdexcept>

#include "pager.h"


FirstPageManager::FirstPageManager(PageNumber page_num, std::unique_ptr<OsFile> db_file_ptr) {
  std::unique_ptr<std::array<std::byte, PAGE_SIZE>> page_content = 
    std::make_unique<std::array<std::byte, PAGE_SIZE>>();
  
  if (db_file_ptr == nullptr)
    throw std::runtime_error("[FirstPageManager]: db_file_ptr is NULL");

  OsFile db_file = *db_file_ptr;
  db_file.read_page(page_num, *page_content);

  PagerFirstPageHeader_t first_page_header;
  std::memcpy(&first_page_header, (*page_content).data(), sizeof(PagerFirstPageHeader_t));
  
  checksum_ = first_page_header.checksum;
  page_type_ = first_page_header.page_type;
  num_pages_ = first_page_header.num_pages;

  std::memcpy(data_.data(),
    (*page_content).data() + sizeof(PagerFirstPageHeader_t),
    PAGE_SIZE - sizeof(PagerFirstPageHeader_t)
  );
}

FirstPageManager::~FirstPageManager() = default;


NodePageManager::NodePageManager(PageNumber page_num, std::unique_ptr<OsFile> db_file_ptr) {
  std::unique_ptr<std::array<std::byte, PAGE_SIZE>> page_content = 
    std::make_unique<std::array<std::byte, PAGE_SIZE>>();

  if (db_file_ptr == nullptr)
    throw std::runtime_error("[NodePageManager]: db_file_ptr is NULL");

  OsFile db_file = *db_file_ptr;
  db_file.read_page(page_num, *page_content);

  PagerNodePageHeader_t node_page_header;
  std::memcpy(&node_page_header, (*page_content).data(), sizeof(PagerNodePageHeader_t));

  checksum_ = node_page_header.checksum;
  page_type_ = node_page_header.page_type;

  std::memcpy(data_.data(),
    (*page_content).data() + sizeof(PagerNodePageHeader_t),  
    PAGE_SIZE - sizeof(PagerNodePageHeader_t)
  );
}

NodePageManager::~NodePageManager() = default;
