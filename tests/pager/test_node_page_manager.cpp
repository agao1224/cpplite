#include <cstddef>
#include <memory>
#include <string>

#include <gtest/gtest.h>
#include "pager/base_page.h"
#include "pager/pager.h"
#include "pager/node_page/node_page.h"
#include "vfs/osfile.h"
#include "test_utils.h"

TEST(NodePageManagerTest, NodePageManagerBasic) {
  const std::string test_db_filename = generate_random_filename();
  std::shared_ptr<OsFile> db_file_ptr = std::make_shared<OsFile>(test_db_filename);

  ASSERT_NE(db_file_ptr, nullptr);

  Pager pager(test_db_filename);
  PageNumber np1 = pager.create_page(PAGER_NODE_PAGE);

  NodePageManager npm(np1, db_file_ptr);
  ASSERT_EQ(npm.get_free_space(), PAGE_SIZE - sizeof(PagerNodePageHeader_t));
  ASSERT_EQ(npm.num_cells_, 0);
  ASSERT_EQ(npm.cells_.size(), 0);
  ASSERT_EQ(npm.pgno_, 2);
}

TEST(NodePageManagerTest, NodePageManagerInsert1) {
  const std::string test_db_filename = generate_random_filename();
  std::shared_ptr<OsFile> db_file_ptr = std::make_shared<OsFile>(test_db_filename);

  std::vector<std::byte> random_payload = generate_random_payload(1000);

  Pager pager(test_db_filename);
  PageNumber np1 = pager.create_page(PAGER_NODE_PAGE);

  NodePageManager npm(np1, db_file_ptr);
  size_t prev_free_space = npm.get_free_space();

  ASSERT_EQ(prev_free_space, PAGE_SIZE - sizeof(PagerNodePageHeader_t));

  bool insert_ok = npm.insert_cell(1, random_payload);
  ASSERT_TRUE(insert_ok);
  size_t new_free_space = npm.get_free_space();
  ASSERT_EQ(npm.num_cells_, 1);

  PagerCell_t cell = npm.cells_[0];
  ASSERT_EQ(cell.key, 1);
  ASSERT_EQ(cell.left_child, NULL_PAGE);
  ASSERT_EQ(cell.size, random_payload.size());
  ASSERT_EQ(cell.record_page, 3);
}
