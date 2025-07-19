#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

#include <gtest/gtest.h>
#include "../../lib/pager/pager.h"
#include "../test_utils.cpp"

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
