#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <random>

#include <gtest/gtest.h>
#include "pager/pager.h"
#include "test_utils.h"

TEST(PagerTest, InitPager) {
  const std::string test_db_filename = generate_random_filename();
  Pager pager(test_db_filename);

  size_t file_size = std::filesystem::file_size(test_db_filename);
  ASSERT_EQ(file_size, PAGE_SIZE);

  std::ifstream db_file(test_db_filename, std::ios::binary);
  ASSERT_TRUE(db_file.is_open());

  std::vector<std::byte> db_contents;
  for (size_t i = 0; i < file_size; i++) {
    char byte;
    db_file.read(&byte, 1);
    db_contents.push_back(std::byte{static_cast<unsigned char>(byte)});
  }

  PagerFirstPageHeader_t first_page_header(db_contents);
  ASSERT_EQ(first_page_header.num_pages, 1);
  ASSERT_EQ(first_page_header.page_type, PAGER_FIRST_PAGE);
  ASSERT_EQ(first_page_header.checksum, CHECKSUM);
  ASSERT_EQ(first_page_header.num_pages, 1);
}

TEST(PagerTest, NodePageBasic) {
  const std::string test_db_filename = generate_random_filename();
  Pager pager(test_db_filename);

  for (size_t i = 0; i < 1000; i++) {
    // NOTE(andrew): starts from page 2, since we count by
    // 1-indexed and first page is already allocated
    ASSERT_EQ(pager.create_node_page(), i+2);
  }
  ASSERT_EQ(std::filesystem::file_size(test_db_filename), 1001*PAGE_SIZE);

  for (size_t i = 0; i < 1000; i++) {
    pager.seek_page(i+2);
    ASSERT_TRUE(pager.page_manager_.has_value());
    auto page_manager = std::get<NodePageManager>(pager.page_manager_.value());
    ASSERT_EQ(page_manager.page_type_, PAGER_NODE_PAGE);
    ASSERT_EQ(page_manager.checksum_, CHECKSUM);
    ASSERT_EQ(page_manager.pgno_, i+2);
  }
}

TEST(PagerTest, FreePageBasic) {
  const std::string test_db_filename = generate_random_filename();
  Pager pager(test_db_filename);

  for (size_t i = 0; i < 1000; i++) {
    ASSERT_EQ(pager.create_free_page(), i+2);
  }
  ASSERT_EQ(std::filesystem::file_size(test_db_filename), 1001*PAGE_SIZE);

  for (size_t i = 0; i < 1000; i++) {
    pager.seek_page(i+2);
    ASSERT_TRUE(pager.page_manager_.has_value());
    auto page_manager = std::get<FreePageManager>(pager.page_manager_.value());
    ASSERT_EQ(page_manager.page_type_, PAGER_FREE_PAGE);
    ASSERT_EQ(page_manager.checksum_, CHECKSUM);
    ASSERT_EQ(page_manager.pgno_, i+2);
  }
}

TEST(PagerTest, FreePageListTraversal) {
  const std::string test_db_filename = generate_random_filename();
  Pager pager(test_db_filename);

  size_t num_free_pages = 0;
  for (size_t i = 0; i < 1000; i++) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<> dist(0, 1);
    bool create_free_page = dist(rng);
    if (create_free_page) {
      pager.create_free_page();
      num_free_pages++;
    } else
      pager.create_node_page();
  }

  // TODO(andrew):
  // create functions for managing page freelist in Pager, then
  // complete freelist traversal test
  //
  // PageNumber free_page_head = pager.get_free_page_head();
  // size_t counted_free_pages = 0;
  // while (free_page_head != NULL_PAGE) {
  //   pager.seek_page(free_page_head);
  //   ASSERT_TRUE(pager.page_manager_.has_value());
  //   FreePageManager fpm = std::get<FreePageManager>(pager.page_manager_.value());
  //   free_page_head = fpm.get_next_free_page();
  // }
}
