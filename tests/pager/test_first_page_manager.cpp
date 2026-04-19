#include <cstddef>
#include <string>
#include <filesystem>

#include <gtest/gtest.h>
#include "vfs/osfile.h"
#include "pager/pager.h"
#include "db_test_fixture.h"
#include "test_utils.h"

class PagerPageManagerTest : public DbTestFixture {};

TEST_F(PagerPageManagerTest, FirstPageManagerBasic) {
  std::shared_ptr<OsFile> db_file_ptr = std::make_shared<OsFile>(db_filename);

  ASSERT_NE(db_file_ptr, nullptr);

  FirstPageManager fpm(db_file_ptr);

  ASSERT_NE(fpm.db_file_ptr_, nullptr);
  ASSERT_EQ(fpm.checksum_, CHECKSUM);
  ASSERT_EQ(fpm.page_type_, PAGER_FIRST_PAGE);
  ASSERT_EQ(fpm.num_pages_, 1);
  ASSERT_EQ(fpm.free_page_head_, NULL_PAGE);
  ASSERT_EQ(fpm.next_oid_, DefaultPagerKey(1));

  ASSERT_EQ(fpm.get_free_page_head(), NULL_PAGE);
  ASSERT_EQ(fpm.get_next_oid(), DefaultPagerKey(1));
}

TEST_F(PagerPageManagerTest, FirstPageManagerSetters) {
  ASSERT_EQ(std::filesystem::file_size(db_filename), PAGE_SIZE);

  pager->seek_page(1);
  FirstPageManager fpm = std::get<FirstPageManager>(pager->page_manager_);
  fpm.set_num_pages(3);
  ASSERT_EQ(fpm.num_pages_, 3);
  fpm.set_free_page_head(10);
  ASSERT_EQ(fpm.free_page_head_, 10);
  ASSERT_EQ(fpm.get_free_page_head(), 10);

  fpm.set_next_oid(DefaultPagerKey(42));
  ASSERT_EQ(fpm.next_oid_, DefaultPagerKey(42));
  ASSERT_EQ(fpm.get_next_oid(), DefaultPagerKey(42));

  std::vector<std::byte> file_bytes = get_file_bytes(db_filename);
  PagerFirstPageHeader_t first_page_header(file_bytes);
  ASSERT_EQ(first_page_header.free_page_head, 10);
  ASSERT_EQ(first_page_header.num_pages, 3);
  ASSERT_EQ(first_page_header.next_oid, DefaultPagerKey(42));
  ASSERT_EQ(first_page_header.page_type, PAGER_FIRST_PAGE);
}
