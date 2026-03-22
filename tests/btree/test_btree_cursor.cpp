#include <cstddef>

#include <gtest/gtest.h>
#include "db_test_fixture.h"
#include "btree/btree.h"

class BTreeCursorTest : public DbTestFixture {};

TEST_F(BTreeCursorTest, BTreeInit) {
  ASSERT_NE(db_file_ptr, nullptr);
  ASSERT_NE(pager, nullptr);

  PageNumber root_pgno = pager->create_page(PAGER_NODE_PAGE);
  BTreeCursor cursor(pager.get(), root_pgno);

  ASSERT_TRUE(cursor.get_cursor_stack().empty());
  ASSERT_EQ(cursor.get_pager(), pager.get());
  ASSERT_EQ(cursor.get_root_pgno(), root_pgno);
}

TEST_F(BTreeCursorTest, BTreeMoveToFirstBasic) {
  ASSERT_NE(db_file_ptr, nullptr);
  ASSERT_NE(pager, nullptr);

  PageNumber root_pgno = pager->create_page(PAGER_NODE_PAGE);
  NodePageManager root_npm(root_pgno, db_file_ptr);

  PageNumber left_child_page;
  for (size_t i = 50; i <= 100; i += 10) {
    PageNumber node_pgno = pager->create_page(PAGER_NODE_PAGE);
    if (i == 50) left_child_page = node_pgno;

    root_npm.insert_node_cell(i);
    root_npm.set_cell_left_child(i, node_pgno);
  }

  NodePageManager left_child_npm(left_child_page, db_file_ptr);
  PageNumber left_leaf_page;
  for (size_t i = 40; i < 50; i++) {
    PageNumber leaf_pgno = pager->create_page(PAGER_LEAF_PAGE);
    if (i == 40) left_leaf_page = leaf_pgno;

    left_child_npm.insert_node_cell(i);
    left_child_npm.set_cell_left_child(i, leaf_pgno);
  }

  LeafPageManager lpm(left_leaf_page, db_file_ptr, pager.get());

  std::vector<std::byte> random_payload = generate_random_payload(1000);
  lpm.insert_cell(10, random_payload);

  BTreeCursor cursor(pager.get(), root_pgno);
  cursor.move_to_first();

  BTreeCursorStack stack = cursor.get_cursor_stack();
  ASSERT_FALSE(stack.empty());
  ASSERT_EQ(stack.size(), 3);

  BTreeCursorStackElt pgno_cell_idx_pair = stack.top();
  ASSERT_EQ(pgno_cell_idx_pair.first, left_leaf_page);
  ASSERT_EQ(pgno_cell_idx_pair.second, 0);

  stack.pop();

  pgno_cell_idx_pair = stack.top();
  ASSERT_EQ(pgno_cell_idx_pair.first, left_child_page);
  ASSERT_EQ(pgno_cell_idx_pair.second, 0);

  stack.pop();

  pgno_cell_idx_pair = stack.top();
  ASSERT_EQ(pgno_cell_idx_pair.first, root_pgno);
  ASSERT_EQ(pgno_cell_idx_pair.second, 0);

  stack.pop();

  ASSERT_TRUE(stack.empty());
}

TEST_F(BTreeCursorTest, BTreeMoveToLastBasic) {
  ASSERT_NE(db_file_ptr, nullptr);
  ASSERT_NE(pager, nullptr);

  PageNumber root_pgno = pager->create_page(PAGER_NODE_PAGE);
  NodePageManager root_npm(root_pgno, db_file_ptr);

  PageNumber right_child_page;
  for (size_t i = 50; i <= 100; i += 10) {
    PageNumber node_pgno = pager->create_page(PAGER_NODE_PAGE);

    if (i < 100) {
      root_npm.insert_node_cell(i);
      root_npm.set_cell_left_child(i, node_pgno);
    } else {
      root_npm.set_node_right_child(node_pgno);
      right_child_page = node_pgno;
    }
  }

  NodePageManager right_child_npm(right_child_page, db_file_ptr);
  PageNumber right_leaf_page;
  for (size_t i = 40; i <= 50; i++) {
    PageNumber leaf_pgno = pager->create_page(PAGER_LEAF_PAGE);

    if (i < 50) {
      right_child_npm.insert_node_cell(i);
      right_child_npm.set_cell_left_child(i, leaf_pgno);
    } else {
      right_child_npm.set_node_right_child(leaf_pgno);
      right_leaf_page = leaf_pgno;
    }
  }

  LeafPageManager lpm(right_leaf_page, db_file_ptr, pager.get());

  std::vector<std::byte> random_payload = generate_random_payload(1000);
  lpm.insert_cell(10, random_payload);
  lpm.insert_cell(11, random_payload);
  lpm.insert_cell(12, random_payload);
  lpm.insert_cell(13, random_payload);

  BTreeCursor cursor(pager.get(), root_pgno);
  cursor.move_to_last();

  BTreeCursorStack stack = cursor.get_cursor_stack();
  ASSERT_FALSE(stack.empty());
  ASSERT_EQ(stack.size(), 3);

  BTreeCursorStackElt pgno_cell_idx_pair = stack.top();
  ASSERT_EQ(pgno_cell_idx_pair.first, right_leaf_page);
  ASSERT_EQ(pgno_cell_idx_pair.second, 3);

  stack.pop();

  pgno_cell_idx_pair = stack.top();
  ASSERT_EQ(pgno_cell_idx_pair.first, right_child_page);
  ASSERT_EQ(pgno_cell_idx_pair.second, 10);

  stack.pop();

  pgno_cell_idx_pair = stack.top();
  ASSERT_EQ(pgno_cell_idx_pair.first, root_pgno);
  ASSERT_EQ(pgno_cell_idx_pair.second, 5);

  stack.pop();

  ASSERT_TRUE(stack.empty());
}
