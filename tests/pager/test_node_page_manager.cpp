#include <cstddef>
#include <memory>
#include <gtest/gtest.h>

#include "db_test_fixture.h"
#include "test_utils.h"
#include "pager/base_page.h"
#include "pager/pager.h"
#include "pager/node_page/node_page.h"

class NodePageManagerTest : public DbTestFixture {};

TEST_F(NodePageManagerTest, NodePageManagerBasic) {
  ASSERT_NE(db_file_ptr, nullptr);

  PageNumber np1 = pager->create_page(PAGER_NODE_PAGE);

  NodePageManager npm(np1, db_file_ptr);
  ASSERT_EQ(npm.get_free_space(), PAGE_SIZE - sizeof(PagerNodePageHeader_t));
  ASSERT_EQ(npm.num_cells_, 0);
  ASSERT_EQ(npm.cells_.size(), 0);
  ASSERT_EQ(npm.pgno_, 2);
}

TEST_F(NodePageManagerTest, NodePageManagerInsert1) {
  std::vector<std::byte> random_payload = generate_random_payload(1000);

  PageNumber np1 = pager->create_page(PAGER_NODE_PAGE);

  NodePageManager npm(np1, db_file_ptr);
  size_t prev_free_space = npm.get_free_space();

  ASSERT_EQ(prev_free_space, PAGE_SIZE - sizeof(PagerNodePageHeader_t));

  bool insert_ok = npm.insert_node_cell(1);
  ASSERT_TRUE(insert_ok);
  size_t new_free_space = npm.get_free_space();
  ASSERT_EQ(npm.num_cells_, 1);

  NodeCell_t cell = npm.cells_[0];
  ASSERT_EQ(cell.key, 1);
  ASSERT_EQ(cell.left_child, NULL_PAGE);
}

TEST_F(NodePageManagerTest, NodePageManagerInsert2) {
  size_t PAYLOAD_SIZE = 100000;
  std::vector<std::byte> random_payload = generate_random_payload(PAYLOAD_SIZE);

  PageNumber np1 = pager->create_page(PAGER_NODE_PAGE);
  NodePageManager npm(np1, db_file_ptr);

  bool insert_ok = npm.insert_node_cell(1);
  ASSERT_TRUE(insert_ok);
}

TEST_F(NodePageManagerTest, NodePageManagerInsert3) {
  PageNumber np1 = pager->create_page(PAGER_NODE_PAGE);
  NodePageManager npm(np1, db_file_ptr);

  for (size_t i = 0; i < 100; i++) {
    bool insert_ok = npm.insert_node_cell(i);
    ASSERT_TRUE(insert_ok);
    ASSERT_EQ(npm.num_cells_, i+1);
  }

  NodePageManager new_npm(np1, db_file_ptr);
  ASSERT_EQ(new_npm.cells_.size(), 100);
  ASSERT_EQ(new_npm.num_cells_, 100);
  ASSERT_EQ(
    new_npm.get_free_space(),
    PAGE_SIZE - sizeof(PagerNodePageHeader_t) - (100 * sizeof(NodeCell_t))
  );
}

TEST_F(NodePageManagerTest, NodePageManagerTree) {
  PageNumber np1 = pager->create_page(PAGER_NODE_PAGE);
  NodePageManager npm1(np1, db_file_ptr);
  for (size_t i = 5; i < 10; i++) {
    bool insert_ok = npm1.insert_node_cell(i);
    ASSERT_TRUE(insert_ok);
    ASSERT_EQ(npm1.num_cells_, (i-5)+1);
  }

  PageNumber np2 = pager->create_page(PAGER_NODE_PAGE);
  NodePageManager npm2(np2, db_file_ptr);
  for (size_t i = 0; i < 5; i++) {
    bool insert_ok = npm2.insert_node_cell(i);
    ASSERT_TRUE(insert_ok);
    ASSERT_EQ(npm2.num_cells_, i+1); 
  }

  PageNumber np3 = pager->create_page(PAGER_NODE_PAGE);
  NodePageManager npm3(np3, db_file_ptr);
  for (size_t i = 10; i < 15; i++) {
    bool insert_ok = npm3.insert_node_cell(i);
    ASSERT_TRUE(insert_ok);
    ASSERT_EQ(npm3.num_cells_, (i-10)+1); 
  }

  PageNumber np4 = pager->create_page(PAGER_NODE_PAGE);
  NodePageManager npm4(np4, db_file_ptr);
  bool insert_ok = npm4.insert_node_cell(99);
  ASSERT_TRUE(insert_ok);
  ASSERT_EQ(npm4.num_cells_, 1);

  npm1.set_cell_left_child(5, np2);
  npm1.set_cell_left_child(9, np4);
  npm1.set_node_right_child(np3);

  NodePageManager npm(np1, db_file_ptr);
  ASSERT_EQ(npm.cells_[0].left_child, np2);
  ASSERT_EQ(npm.right_child_, np3);
  ASSERT_EQ(npm.cells_[4].left_child, np4);
}
