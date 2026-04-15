#include <cstddef>
#include <string>

#include <gtest/gtest.h>
#include "db_test_fixture.h"
#include "btree/btree_test_utils.h"
#include "btree/btree.h"
#include "btree/builder/btree_builder.h"
#include "pager/leaf_page/leaf_page.h"

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

TEST_F(BTreeCursorTest, BTreeMoveToFirstSimple) {
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

  BTreeCursorNode pgno_cell_idx_pair = stack.top();
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

TEST_F(BTreeCursorTest, BTreeMoveToLastSimple) {
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

  BTreeCursorNode pgno_cell_idx_pair = stack.top();
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

TEST_F(BTreeCursorTest, BTreeMoveToFirstAndLastDeep1) {
  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 1;
  config.node_max_cells = config.leaf_max_cells = 2;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/deep1.yaml",
    pager.get(),
    config
  );

  BTreeCursor cursor(pager.get(), root_pgno, config);
  cursor.move_to_first();

  BTreeCursorStack stack = cursor.get_cursor_stack();
  BTreeCursorNode curr = stack.top();

  ASSERT_EQ(pager->get_page_type(curr.first), PAGER_LEAF_PAGE);

  LeafPageManager lpm(curr.first, db_file_ptr, pager.get());
  ASSERT_TRUE(lpm.cells_.size() == 2 && lpm.cells_.size() == lpm.num_cells_);
  ASSERT_EQ(curr.second, 0);

  LeafCell_t curr_cell = lpm.cells_[curr.second];
  ASSERT_EQ(curr_cell.key, 1);
  ASSERT_EQ(curr_cell.payload_size, sizeof("1")-1);
  assert_payload(curr_cell.record_page, db_file_ptr, "1");

  ASSERT_EQ(stack.size(), 4);

  cursor.move_to_last();
  stack = cursor.get_cursor_stack();
  curr = stack.top();

  ASSERT_EQ(pager->get_page_type(curr.first), PAGER_LEAF_PAGE);

  lpm = LeafPageManager(curr.first, db_file_ptr, pager.get());
  ASSERT_TRUE(lpm.cells_.size() == 2 && lpm.cells_.size() == lpm.num_cells_);
  ASSERT_EQ(curr.second, 1);

  curr_cell = lpm.cells_[curr.second];
  ASSERT_EQ(curr_cell.key, 20);
  ASSERT_EQ(curr_cell.payload_size, sizeof("20")-1);
  assert_payload(curr_cell.record_page, db_file_ptr, "20");

  ASSERT_EQ(stack.size(), 4);
}

TEST_F(BTreeCursorTest, BTreeMoveToFirstAndLastBasic1) {
  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 2;
  config.node_max_cells = config.leaf_max_cells = 4;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/basic1.yaml",
    pager.get(),
    config
  );

  BTreeCursor cursor(pager.get(), root_pgno, config);
  cursor.move_to_first();

  BTreeCursorStack stack = cursor.get_cursor_stack();
  BTreeCursorNode curr = stack.top();

  ASSERT_EQ(pager->get_page_type(curr.first), PAGER_LEAF_PAGE);

  LeafPageManager lpm(curr.first, db_file_ptr, pager.get());
  ASSERT_TRUE(lpm.cells_.size() == 2 && lpm.cells_.size() == lpm.num_cells_);
  ASSERT_EQ(curr.second, 0);

  LeafCell_t curr_cell = lpm.cells_[curr.second];
  ASSERT_EQ(curr_cell.key, 1);
  ASSERT_EQ(curr_cell.payload_size, sizeof("111")-1);
  assert_payload(curr_cell.record_page, db_file_ptr, "111");

  ASSERT_EQ(stack.size(), 2);

  cursor.move_to_last();
  stack = cursor.get_cursor_stack();
  curr = stack.top();

  ASSERT_EQ(pager->get_page_type(curr.first), PAGER_LEAF_PAGE);

  lpm = LeafPageManager(curr.first, db_file_ptr, pager.get());
  ASSERT_TRUE(lpm.cells_.size() == 2 && lpm.cells_.size() == lpm.num_cells_);
  ASSERT_EQ(curr.second, 1);

  curr_cell = lpm.cells_[curr.second];
  ASSERT_EQ(curr_cell.key, 13);
  ASSERT_EQ(curr_cell.payload_size, sizeof("131313")-1);
  assert_payload(curr_cell.record_page, db_file_ptr, "131313");

  ASSERT_EQ(stack.size(), 2);
}

TEST_F(BTreeCursorTest, BTreeMoveToKeyBasic1) {
  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 2;
  config.node_max_cells = config.leaf_max_cells = 4;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/basic1.yaml",
    pager.get(),
    config
  );

  BTreeCursor cursor(pager.get(), root_pgno, config);
  DefaultPagerKey curr_key;

  ASSERT_TRUE(cursor.move_to_key(1));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 1);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "111");

  ASSERT_TRUE(cursor.move_to_key(3));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 3);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "333");

  ASSERT_TRUE(cursor.move_to_key(5));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 5);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "555");

  ASSERT_TRUE(cursor.move_to_key(9));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 9);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "999");

  ASSERT_TRUE(cursor.move_to_key(10));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 10);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "101010");

  ASSERT_TRUE(cursor.move_to_key(12));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 12);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "121212");

  ASSERT_TRUE(cursor.move_to_key(13));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 13);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "131313");

  ASSERT_FALSE(cursor.move_to_key(4));
  ASSERT_FALSE(cursor.move_to_key(99));
}

TEST_F(BTreeCursorTest, BTreeMoveToKeyBasic2) {
  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 2;
  config.node_max_cells = config.leaf_max_cells = 4;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/basic2.yaml",
    pager.get(),
    config
  );

  BTreeCursor cursor(pager.get(), root_pgno, config);
  DefaultPagerKey curr_key;

  ASSERT_TRUE(cursor.move_to_key(1));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 1);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "111");

  ASSERT_TRUE(cursor.move_to_key(3));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 3);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "333");

  ASSERT_TRUE(cursor.move_to_key(5));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 5);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "555");

  ASSERT_TRUE(cursor.move_to_key(7));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 7);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "777");

  ASSERT_TRUE(cursor.move_to_key(9));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 9);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "999");

  ASSERT_TRUE(cursor.move_to_key(11));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 11);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "111111");

  ASSERT_TRUE(cursor.move_to_key(13));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 13);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "131313");

  ASSERT_TRUE(cursor.move_to_key(14));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 14);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "141414");

  ASSERT_TRUE(cursor.move_to_key(15));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 15);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "151515");

  ASSERT_TRUE(cursor.move_to_key(20));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 20);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "202020");

  ASSERT_TRUE(cursor.move_to_key(21));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 21);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "212121");

  ASSERT_TRUE(cursor.move_to_key(23));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 23);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "232323");

  ASSERT_FALSE(cursor.move_to_key(19));

  ASSERT_FALSE(cursor.move_to_key(2));
  ASSERT_FALSE(cursor.move_to_key(100));
}

TEST_F(BTreeCursorTest, BTreeMoveToKeyBasic3) {
  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 2;
  config.node_max_cells = config.leaf_max_cells = 4;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/basic3.yaml",
    pager.get(),
    config
  );

  BTreeCursor cursor(pager.get(), root_pgno, config);
  DefaultPagerKey curr_key;

  ASSERT_TRUE(cursor.move_to_key(1));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 1);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "111");

  ASSERT_TRUE(cursor.move_to_key(3));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 3);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "333");

  ASSERT_TRUE(cursor.move_to_key(5));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 5);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "555");

  ASSERT_TRUE(cursor.move_to_key(7));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 7);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "777");

  ASSERT_TRUE(cursor.move_to_key(9));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 9);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "999");

  ASSERT_TRUE(cursor.move_to_key(12));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 12);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "121212");

  ASSERT_TRUE(cursor.move_to_key(14));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 14);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "141414");

  ASSERT_FALSE(cursor.move_to_key(4));

  ASSERT_FALSE(cursor.move_to_key(2));
  ASSERT_FALSE(cursor.move_to_key(100));
}

TEST_F(BTreeCursorTest, BTreeMoveToKeyDeep1) {
  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 1;
  config.node_max_cells = config.leaf_max_cells = 2;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/deep1.yaml",
    pager.get(),
    config
  );

  BTreeCursor cursor(pager.get(), root_pgno, config);
  DefaultPagerKey curr_key;

  ASSERT_TRUE(cursor.move_to_key(1));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 1);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "1");

  ASSERT_TRUE(cursor.move_to_key(2));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 2);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "2");

  ASSERT_TRUE(cursor.move_to_key(3));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 3);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "3");

  ASSERT_TRUE(cursor.move_to_key(4));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 4);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "4");

  ASSERT_TRUE(cursor.move_to_key(5));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 5);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "5");

  ASSERT_TRUE(cursor.move_to_key(7));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 7);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "7");

  ASSERT_TRUE(cursor.move_to_key(8));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 8);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "8");

  ASSERT_TRUE(cursor.move_to_key(9));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 9);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "9");

  ASSERT_TRUE(cursor.move_to_key(10));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 10);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "10");

  ASSERT_TRUE(cursor.move_to_key(12));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 12);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "12");

  ASSERT_TRUE(cursor.move_to_key(13));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 13);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "13");

  ASSERT_TRUE(cursor.move_to_key(14));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 14);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "14");

  ASSERT_TRUE(cursor.move_to_key(15));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 15);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "15");

  ASSERT_TRUE(cursor.move_to_key(17));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 17);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "17");

  ASSERT_TRUE(cursor.move_to_key(18));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 18);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "18");

  ASSERT_TRUE(cursor.move_to_key(20));
  curr_key = cursor.current_key();
  ASSERT_EQ(curr_key, 20);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "20");

  ASSERT_FALSE(cursor.move_to_key(6));
  ASSERT_FALSE(cursor.move_to_key(11));
  ASSERT_FALSE(cursor.move_to_key(100));
}

TEST_F(BTreeCursorTest, BTreePrevBasic1) {
  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 2;
  config.node_max_cells = config.leaf_max_cells = 4;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/basic1.yaml",
    pager.get(),
    config
  );

  BTreeCursor cursor(pager.get(), root_pgno, config);
  cursor.move_to_last();

  ASSERT_EQ(cursor.current_key(), 13);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "131313");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 12);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "121212");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 10);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "101010");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 9);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "999");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 5);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "555");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 3);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "333");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 1);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "111");

  ASSERT_FALSE(cursor.prev());
}

TEST_F(BTreeCursorTest, BTreePrevBasic2) {
  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 2;
  config.node_max_cells = config.leaf_max_cells = 4;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/basic2.yaml",
    pager.get(),
    config
  );

  BTreeCursor cursor(pager.get(), root_pgno, config);
  cursor.move_to_last();

  ASSERT_EQ(cursor.current_key(), 23);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "232323");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 21);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "212121");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 20);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "202020");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 15);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "151515");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 14);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "141414");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 13);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "131313");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 11);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "111111");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 9);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "999");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 7);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "777");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 5);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "555");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 3);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "333");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 1);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "111");

  ASSERT_FALSE(cursor.prev());
}

TEST_F(BTreeCursorTest, BTreePrevBasic3) {
  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 2;
  config.node_max_cells = config.leaf_max_cells = 4;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/basic3.yaml",
    pager.get(),
    config
  );

  BTreeCursor cursor(pager.get(), root_pgno, config);
  cursor.move_to_last();

  ASSERT_EQ(cursor.current_key(), 14);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "141414");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 12);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "121212");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 9);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "999");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 7);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "777");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 5);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "555");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 3);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "333");

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 1);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "111");

  ASSERT_FALSE(cursor.prev());
}

TEST_F(BTreeCursorTest, BTreePrevDeep1) {
  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 1;
  config.node_max_cells = config.leaf_max_cells = 2;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/deep1.yaml",
    pager.get(),
    config
  );

  BTreeCursor cursor(pager.get(), root_pgno, config);
  BTreeCursorStack s;

  cursor.move_to_key(1);
  s = cursor.get_cursor_stack();
  PageNumber pgno_leaf1 = s.top().first; s.pop();
  PageNumber pgno_ll    = s.top().first; s.pop();
  PageNumber pgno_left  = s.top().first; s.pop();

  cursor.move_to_key(3);
  s = cursor.get_cursor_stack();
  PageNumber pgno_leaf2 = s.top().first;

  cursor.move_to_key(5);
  s = cursor.get_cursor_stack();
  PageNumber pgno_leaf3 = s.top().first; s.pop();
  PageNumber pgno_lr    = s.top().first;

  cursor.move_to_key(8);
  s = cursor.get_cursor_stack();
  PageNumber pgno_leaf4 = s.top().first;

  cursor.move_to_key(10);
  s = cursor.get_cursor_stack();
  PageNumber pgno_leaf5 = s.top().first; s.pop();
  PageNumber pgno_rl    = s.top().first; s.pop();
  PageNumber pgno_right = s.top().first;

  cursor.move_to_key(13);
  s = cursor.get_cursor_stack();
  PageNumber pgno_leaf6 = s.top().first;

  cursor.move_to_key(15);
  s = cursor.get_cursor_stack();
  PageNumber pgno_leaf7 = s.top().first; s.pop();
  PageNumber pgno_rr    = s.top().first;

  cursor.move_to_key(18);
  s = cursor.get_cursor_stack();
  PageNumber pgno_leaf8 = s.top().first;

  cursor.move_to_last();

  ASSERT_EQ(cursor.current_key(), 20);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "20");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf8, 1}, {pgno_rr, 1}, {pgno_right, 1}, {root_pgno, 1}
  });

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 18);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "18");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf8, 0}, {pgno_rr, 1}, {pgno_right, 1}, {root_pgno, 1}
  });

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 17);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "17");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf7, 1}, {pgno_rr, 0}, {pgno_right, 1}, {root_pgno, 1}
  });

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 15);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "15");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf7, 0}, {pgno_rr, 0}, {pgno_right, 1}, {root_pgno, 1}
  });

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 14);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "14");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf6, 1}, {pgno_rl, 1}, {pgno_right, 0}, {root_pgno, 1}
  });

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 13);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "13");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf6, 0}, {pgno_rl, 1}, {pgno_right, 0}, {root_pgno, 1}
  });

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 12);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "12");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf5, 1}, {pgno_rl, 0}, {pgno_right, 0}, {root_pgno, 1}
  });

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 10);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "10");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf5, 0}, {pgno_rl, 0}, {pgno_right, 0}, {root_pgno, 1}
  });

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 9);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "9");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf4, 1}, {pgno_lr, 1}, {pgno_left, 1}, {root_pgno, 0}
  });

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 8);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "8");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf4, 0}, {pgno_lr, 1}, {pgno_left, 1}, {root_pgno, 0}
  });

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 7);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "7");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf3, 1}, {pgno_lr, 0}, {pgno_left, 1}, {root_pgno, 0}
  });

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 5);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "5");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf3, 0}, {pgno_lr, 0}, {pgno_left, 1}, {root_pgno, 0}
  });

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 4);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "4");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf2, 1}, {pgno_ll, 1}, {pgno_left, 0}, {root_pgno, 0}
  });

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 3);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "3");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf2, 0}, {pgno_ll, 1}, {pgno_left, 0}, {root_pgno, 0}
  });

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 2);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "2");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf1, 1}, {pgno_ll, 0}, {pgno_left, 0}, {root_pgno, 0}
  });

  ASSERT_TRUE(cursor.prev());
  ASSERT_EQ(cursor.current_key(), 1);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "1");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf1, 0}, {pgno_ll, 0}, {pgno_left, 0}, {root_pgno, 0}
  });

  ASSERT_FALSE(cursor.prev());
}

TEST_F(BTreeCursorTest, BTreeNextBasic1) {
  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 2;
  config.node_max_cells = config.leaf_max_cells = 4;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/basic1.yaml",
    pager.get(),
    config
  );

  BTreeCursor cursor(pager.get(), root_pgno, config);
  cursor.move_to_first();

  ASSERT_EQ(cursor.current_key(), 1);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "111");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 3);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "333");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 5);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "555");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 9);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "999");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 10);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "101010");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 12);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "121212");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 13);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "131313");

  ASSERT_FALSE(cursor.next());
}

TEST_F(BTreeCursorTest, BTreeNextBasic2) {
  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 2;
  config.node_max_cells = config.leaf_max_cells = 4;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/basic2.yaml",
    pager.get(),
    config
  );

  BTreeCursor cursor(pager.get(), root_pgno, config);
  cursor.move_to_first();

  ASSERT_EQ(cursor.current_key(), 1);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "111");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 3);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "333");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 5);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "555");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 7);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "777");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 9);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "999");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 11);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "111111");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 13);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "131313");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 14);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "141414");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 15);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "151515");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 20);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "202020");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 21);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "212121");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 23);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "232323");

  ASSERT_FALSE(cursor.next());
}

TEST_F(BTreeCursorTest, BTreeNextBasic3) {
  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 2;
  config.node_max_cells = config.leaf_max_cells = 4;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/basic3.yaml",
    pager.get(),
    config
  );

  BTreeCursor cursor(pager.get(), root_pgno, config);
  cursor.move_to_first();

  ASSERT_EQ(cursor.current_key(), 1);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "111");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 3);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "333");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 5);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "555");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 7);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "777");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 9);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "999");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 12);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "121212");

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 14);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "141414");

  ASSERT_FALSE(cursor.next());
}

TEST_F(BTreeCursorTest, BTreeNextDeep1) {
  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 1;
  config.node_max_cells = config.leaf_max_cells = 2;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/deep1.yaml",
    pager.get(),
    config
  );

  BTreeCursor cursor(pager.get(), root_pgno, config);

  BTreeCursorStack s;

  cursor.move_to_key(1);
  s = cursor.get_cursor_stack();
  PageNumber pgno_leaf1 = s.top().first; s.pop();
  PageNumber pgno_ll    = s.top().first; s.pop();
  PageNumber pgno_left  = s.top().first;

  cursor.move_to_key(3);
  s = cursor.get_cursor_stack();
  PageNumber pgno_leaf2 = s.top().first;

  cursor.move_to_key(5);
  s = cursor.get_cursor_stack();
  PageNumber pgno_leaf3 = s.top().first; s.pop();
  PageNumber pgno_lr    = s.top().first;

  cursor.move_to_key(8);
  s = cursor.get_cursor_stack();
  PageNumber pgno_leaf4 = s.top().first;

  cursor.move_to_key(10);
  s = cursor.get_cursor_stack();
  PageNumber pgno_leaf5 = s.top().first; s.pop();
  PageNumber pgno_rl    = s.top().first; s.pop();
  PageNumber pgno_right = s.top().first;

  cursor.move_to_key(13);
  s = cursor.get_cursor_stack();
  PageNumber pgno_leaf6 = s.top().first;

  cursor.move_to_key(15);
  s = cursor.get_cursor_stack();
  PageNumber pgno_leaf7 = s.top().first; s.pop();
  PageNumber pgno_rr    = s.top().first;

  cursor.move_to_key(18);
  s = cursor.get_cursor_stack();
  PageNumber pgno_leaf8 = s.top().first;

  cursor.move_to_first();

  ASSERT_EQ(cursor.current_key(), 1);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "1");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf1, 0}, {pgno_ll, 0}, {pgno_left, 0}, {root_pgno, 0}
  });

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 2);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "2");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf1, 1}, {pgno_ll, 0}, {pgno_left, 0}, {root_pgno, 0}
  });

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 3);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "3");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf2, 0}, {pgno_ll, 1}, {pgno_left, 0}, {root_pgno, 0}
  });

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 4);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "4");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf2, 1}, {pgno_ll, 1}, {pgno_left, 0}, {root_pgno, 0}
  });

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 5);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "5");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf3, 0}, {pgno_lr, 0}, {pgno_left, 1}, {root_pgno, 0}
  });

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 7);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "7");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf3, 1}, {pgno_lr, 0}, {pgno_left, 1}, {root_pgno, 0}
  });

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 8);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "8");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf4, 0}, {pgno_lr, 1}, {pgno_left, 1}, {root_pgno, 0}
  });

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 9);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "9");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf4, 1}, {pgno_lr, 1}, {pgno_left, 1}, {root_pgno, 0}
  });

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 10);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "10");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf5, 0}, {pgno_rl, 0}, {pgno_right, 0}, {root_pgno, 1}
  });

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 12);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "12");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf5, 1}, {pgno_rl, 0}, {pgno_right, 0}, {root_pgno, 1}
  });

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 13);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "13");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf6, 0}, {pgno_rl, 1}, {pgno_right, 0}, {root_pgno, 1}
  });

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 14);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "14");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf6, 1}, {pgno_rl, 1}, {pgno_right, 0}, {root_pgno, 1}
  });

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 15);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "15");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf7, 0}, {pgno_rr, 0}, {pgno_right, 1}, {root_pgno, 1}
  });

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 17);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "17");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf7, 1}, {pgno_rr, 0}, {pgno_right, 1}, {root_pgno, 1}
  });

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 18);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "18");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf8, 0}, {pgno_rr, 1}, {pgno_right, 1}, {root_pgno, 1}
  });

  ASSERT_TRUE(cursor.next());
  ASSERT_EQ(cursor.current_key(), 20);
  assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, "20");
  assert_cursor_stack(cursor.get_cursor_stack(), {
    {pgno_leaf8, 1}, {pgno_rr, 1}, {pgno_right, 1}, {root_pgno, 1}
  });

  ASSERT_FALSE(cursor.next());
}

TEST_F(BTreeCursorTest, BTreeNextDeep2) {
  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 3;
  config.node_max_cells = config.leaf_max_cells = 6;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/deep2.yaml",
    pager.get(),
    config
  );

  BTreeCursor cursor(pager.get(), root_pgno, config);

  PageNumber leaf_pgno[46];
  BTreeCursorStack s;

  cursor.move_to_key(1);  s = cursor.get_cursor_stack(); leaf_pgno[1] = s.top().first; s.pop(); PageNumber pgno_a1    = s.top().first; s.pop(); PageNumber pgno_left  = s.top().first;
  cursor.move_to_key(4);  s = cursor.get_cursor_stack(); leaf_pgno[2]  = s.top().first;
  cursor.move_to_key(7);  s = cursor.get_cursor_stack(); leaf_pgno[3]  = s.top().first;
  cursor.move_to_key(10); s = cursor.get_cursor_stack(); leaf_pgno[4]  = s.top().first;

  cursor.move_to_key(13); s = cursor.get_cursor_stack(); leaf_pgno[5]  = s.top().first; s.pop(); PageNumber pgno_a2 = s.top().first;
  cursor.move_to_key(16); s = cursor.get_cursor_stack(); leaf_pgno[6]  = s.top().first;
  cursor.move_to_key(19); s = cursor.get_cursor_stack(); leaf_pgno[7]  = s.top().first;
  cursor.move_to_key(22); s = cursor.get_cursor_stack(); leaf_pgno[8]  = s.top().first;
  cursor.move_to_key(25); s = cursor.get_cursor_stack(); leaf_pgno[9]  = s.top().first;

  cursor.move_to_key(28); s = cursor.get_cursor_stack(); leaf_pgno[10] = s.top().first; s.pop(); PageNumber pgno_a3 = s.top().first;
  cursor.move_to_key(31); s = cursor.get_cursor_stack(); leaf_pgno[11] = s.top().first;
  cursor.move_to_key(34); s = cursor.get_cursor_stack(); leaf_pgno[12] = s.top().first;
  cursor.move_to_key(37); s = cursor.get_cursor_stack(); leaf_pgno[13] = s.top().first;
  cursor.move_to_key(40); s = cursor.get_cursor_stack(); leaf_pgno[14] = s.top().first;
  cursor.move_to_key(43); s = cursor.get_cursor_stack(); leaf_pgno[15] = s.top().first;

  cursor.move_to_key(46); s = cursor.get_cursor_stack(); leaf_pgno[16] = s.top().first; s.pop(); PageNumber pgno_a4 = s.top().first;
  cursor.move_to_key(49); s = cursor.get_cursor_stack(); leaf_pgno[17] = s.top().first;
  cursor.move_to_key(52); s = cursor.get_cursor_stack(); leaf_pgno[18] = s.top().first;
  cursor.move_to_key(55); s = cursor.get_cursor_stack(); leaf_pgno[19] = s.top().first;

  cursor.move_to_key(58); s = cursor.get_cursor_stack(); leaf_pgno[20] = s.top().first; s.pop(); PageNumber pgno_b1 = s.top().first; s.pop(); PageNumber pgno_right = s.top().first;
  cursor.move_to_key(61); s = cursor.get_cursor_stack(); leaf_pgno[21] = s.top().first;
  cursor.move_to_key(64); s = cursor.get_cursor_stack(); leaf_pgno[22] = s.top().first;
  cursor.move_to_key(67); s = cursor.get_cursor_stack(); leaf_pgno[23] = s.top().first;

  cursor.move_to_key(70); s = cursor.get_cursor_stack(); leaf_pgno[24] = s.top().first; s.pop(); PageNumber pgno_b2 = s.top().first;
  cursor.move_to_key(73); s = cursor.get_cursor_stack(); leaf_pgno[25] = s.top().first;
  cursor.move_to_key(76); s = cursor.get_cursor_stack(); leaf_pgno[26] = s.top().first;
  cursor.move_to_key(79); s = cursor.get_cursor_stack(); leaf_pgno[27] = s.top().first;
  cursor.move_to_key(82); s = cursor.get_cursor_stack(); leaf_pgno[28] = s.top().first;
  cursor.move_to_key(85); s = cursor.get_cursor_stack(); leaf_pgno[29] = s.top().first;

  cursor.move_to_key(88);  s = cursor.get_cursor_stack(); leaf_pgno[30] = s.top().first; s.pop(); PageNumber pgno_b3 = s.top().first;
  cursor.move_to_key(91);  s = cursor.get_cursor_stack(); leaf_pgno[31] = s.top().first;
  cursor.move_to_key(94);  s = cursor.get_cursor_stack(); leaf_pgno[32] = s.top().first;
  cursor.move_to_key(97);  s = cursor.get_cursor_stack(); leaf_pgno[33] = s.top().first;
  cursor.move_to_key(100); s = cursor.get_cursor_stack(); leaf_pgno[34] = s.top().first;

  cursor.move_to_key(103); s = cursor.get_cursor_stack(); leaf_pgno[35] = s.top().first; s.pop(); PageNumber pgno_b4 = s.top().first;
  cursor.move_to_key(106); s = cursor.get_cursor_stack(); leaf_pgno[36] = s.top().first;
  cursor.move_to_key(109); s = cursor.get_cursor_stack(); leaf_pgno[37] = s.top().first;
  cursor.move_to_key(112); s = cursor.get_cursor_stack(); leaf_pgno[38] = s.top().first;
  cursor.move_to_key(115); s = cursor.get_cursor_stack(); leaf_pgno[39] = s.top().first;
  cursor.move_to_key(118); s = cursor.get_cursor_stack(); leaf_pgno[40] = s.top().first;
  cursor.move_to_key(121); s = cursor.get_cursor_stack(); leaf_pgno[41] = s.top().first;

  cursor.move_to_key(124); s = cursor.get_cursor_stack(); leaf_pgno[42] = s.top().first; s.pop(); PageNumber pgno_b5 = s.top().first;
  cursor.move_to_key(127); s = cursor.get_cursor_stack(); leaf_pgno[43] = s.top().first;
  cursor.move_to_key(130); s = cursor.get_cursor_stack(); leaf_pgno[44] = s.top().first;
  cursor.move_to_key(133); s = cursor.get_cursor_stack(); leaf_pgno[45] = s.top().first;

  auto check = [&](int key, int ln, size_t lci,
                   PageNumber l3p, size_t l3i,
                   PageNumber l2p, size_t l2i,
                   size_t ri) {
    ASSERT_EQ(cursor.current_key(), key);
    assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, std::to_string(key));
    assert_cursor_stack(cursor.get_cursor_stack(), {
      {leaf_pgno[ln], lci}, {l3p, l3i}, {l2p, l2i}, {root_pgno, ri}
    });
  };

  cursor.move_to_first();

  check(  1,  1, 0, pgno_a1, 0, pgno_left, 0, 0); ASSERT_TRUE(cursor.next());
  check(  2,  1, 1, pgno_a1, 0, pgno_left, 0, 0); ASSERT_TRUE(cursor.next());
  check(  3,  1, 2, pgno_a1, 0, pgno_left, 0, 0); ASSERT_TRUE(cursor.next());
  check(  4,  2, 0, pgno_a1, 1, pgno_left, 0, 0); ASSERT_TRUE(cursor.next());
  check(  5,  2, 1, pgno_a1, 1, pgno_left, 0, 0); ASSERT_TRUE(cursor.next());
  check(  6,  2, 2, pgno_a1, 1, pgno_left, 0, 0); ASSERT_TRUE(cursor.next());
  check(  7,  3, 0, pgno_a1, 2, pgno_left, 0, 0); ASSERT_TRUE(cursor.next());
  check(  8,  3, 1, pgno_a1, 2, pgno_left, 0, 0); ASSERT_TRUE(cursor.next());
  check(  9,  3, 2, pgno_a1, 2, pgno_left, 0, 0); ASSERT_TRUE(cursor.next());
  check( 10,  4, 0, pgno_a1, 3, pgno_left, 0, 0); ASSERT_TRUE(cursor.next());
  check( 11,  4, 1, pgno_a1, 3, pgno_left, 0, 0); ASSERT_TRUE(cursor.next());
  check( 12,  4, 2, pgno_a1, 3, pgno_left, 0, 0); ASSERT_TRUE(cursor.next());

  check( 13,  5, 0, pgno_a2, 0, pgno_left, 1, 0); ASSERT_TRUE(cursor.next());
  check( 14,  5, 1, pgno_a2, 0, pgno_left, 1, 0); ASSERT_TRUE(cursor.next());
  check( 15,  5, 2, pgno_a2, 0, pgno_left, 1, 0); ASSERT_TRUE(cursor.next());
  check( 16,  6, 0, pgno_a2, 1, pgno_left, 1, 0); ASSERT_TRUE(cursor.next());
  check( 17,  6, 1, pgno_a2, 1, pgno_left, 1, 0); ASSERT_TRUE(cursor.next());
  check( 18,  6, 2, pgno_a2, 1, pgno_left, 1, 0); ASSERT_TRUE(cursor.next());
  check( 19,  7, 0, pgno_a2, 2, pgno_left, 1, 0); ASSERT_TRUE(cursor.next());
  check( 20,  7, 1, pgno_a2, 2, pgno_left, 1, 0); ASSERT_TRUE(cursor.next());
  check( 21,  7, 2, pgno_a2, 2, pgno_left, 1, 0); ASSERT_TRUE(cursor.next());
  check( 22,  8, 0, pgno_a2, 3, pgno_left, 1, 0); ASSERT_TRUE(cursor.next());
  check( 23,  8, 1, pgno_a2, 3, pgno_left, 1, 0); ASSERT_TRUE(cursor.next());
  check( 24,  8, 2, pgno_a2, 3, pgno_left, 1, 0); ASSERT_TRUE(cursor.next());
  check( 25,  9, 0, pgno_a2, 4, pgno_left, 1, 0); ASSERT_TRUE(cursor.next());
  check( 26,  9, 1, pgno_a2, 4, pgno_left, 1, 0); ASSERT_TRUE(cursor.next());
  check( 27,  9, 2, pgno_a2, 4, pgno_left, 1, 0); ASSERT_TRUE(cursor.next());

  check( 28, 10, 0, pgno_a3, 0, pgno_left, 2, 0); ASSERT_TRUE(cursor.next());
  check( 29, 10, 1, pgno_a3, 0, pgno_left, 2, 0); ASSERT_TRUE(cursor.next());
  check( 30, 10, 2, pgno_a3, 0, pgno_left, 2, 0); ASSERT_TRUE(cursor.next());
  check( 31, 11, 0, pgno_a3, 1, pgno_left, 2, 0); ASSERT_TRUE(cursor.next());
  check( 32, 11, 1, pgno_a3, 1, pgno_left, 2, 0); ASSERT_TRUE(cursor.next());
  check( 33, 11, 2, pgno_a3, 1, pgno_left, 2, 0); ASSERT_TRUE(cursor.next());
  check( 34, 12, 0, pgno_a3, 2, pgno_left, 2, 0); ASSERT_TRUE(cursor.next());
  check( 35, 12, 1, pgno_a3, 2, pgno_left, 2, 0); ASSERT_TRUE(cursor.next());
  check( 36, 12, 2, pgno_a3, 2, pgno_left, 2, 0); ASSERT_TRUE(cursor.next());
  check( 37, 13, 0, pgno_a3, 3, pgno_left, 2, 0); ASSERT_TRUE(cursor.next());
  check( 38, 13, 1, pgno_a3, 3, pgno_left, 2, 0); ASSERT_TRUE(cursor.next());
  check( 39, 13, 2, pgno_a3, 3, pgno_left, 2, 0); ASSERT_TRUE(cursor.next());
  check( 40, 14, 0, pgno_a3, 4, pgno_left, 2, 0); ASSERT_TRUE(cursor.next());
  check( 41, 14, 1, pgno_a3, 4, pgno_left, 2, 0); ASSERT_TRUE(cursor.next());
  check( 42, 14, 2, pgno_a3, 4, pgno_left, 2, 0); ASSERT_TRUE(cursor.next());
  check( 43, 15, 0, pgno_a3, 5, pgno_left, 2, 0); ASSERT_TRUE(cursor.next());
  check( 44, 15, 1, pgno_a3, 5, pgno_left, 2, 0); ASSERT_TRUE(cursor.next());
  check( 45, 15, 2, pgno_a3, 5, pgno_left, 2, 0); ASSERT_TRUE(cursor.next());

  check( 46, 16, 0, pgno_a4, 0, pgno_left, 3, 0); ASSERT_TRUE(cursor.next());
  check( 47, 16, 1, pgno_a4, 0, pgno_left, 3, 0); ASSERT_TRUE(cursor.next());
  check( 48, 16, 2, pgno_a4, 0, pgno_left, 3, 0); ASSERT_TRUE(cursor.next());
  check( 49, 17, 0, pgno_a4, 1, pgno_left, 3, 0); ASSERT_TRUE(cursor.next());
  check( 50, 17, 1, pgno_a4, 1, pgno_left, 3, 0); ASSERT_TRUE(cursor.next());
  check( 51, 17, 2, pgno_a4, 1, pgno_left, 3, 0); ASSERT_TRUE(cursor.next());
  check( 52, 18, 0, pgno_a4, 2, pgno_left, 3, 0); ASSERT_TRUE(cursor.next());
  check( 53, 18, 1, pgno_a4, 2, pgno_left, 3, 0); ASSERT_TRUE(cursor.next());
  check( 54, 18, 2, pgno_a4, 2, pgno_left, 3, 0); ASSERT_TRUE(cursor.next());
  check( 55, 19, 0, pgno_a4, 3, pgno_left, 3, 0); ASSERT_TRUE(cursor.next());
  check( 56, 19, 1, pgno_a4, 3, pgno_left, 3, 0); ASSERT_TRUE(cursor.next());
  check( 57, 19, 2, pgno_a4, 3, pgno_left, 3, 0); ASSERT_TRUE(cursor.next());

  check( 58, 20, 0, pgno_b1, 0, pgno_right, 0, 1); ASSERT_TRUE(cursor.next());
  check( 59, 20, 1, pgno_b1, 0, pgno_right, 0, 1); ASSERT_TRUE(cursor.next());
  check( 60, 20, 2, pgno_b1, 0, pgno_right, 0, 1); ASSERT_TRUE(cursor.next());
  check( 61, 21, 0, pgno_b1, 1, pgno_right, 0, 1); ASSERT_TRUE(cursor.next());
  check( 62, 21, 1, pgno_b1, 1, pgno_right, 0, 1); ASSERT_TRUE(cursor.next());
  check( 63, 21, 2, pgno_b1, 1, pgno_right, 0, 1); ASSERT_TRUE(cursor.next());
  check( 64, 22, 0, pgno_b1, 2, pgno_right, 0, 1); ASSERT_TRUE(cursor.next());
  check( 65, 22, 1, pgno_b1, 2, pgno_right, 0, 1); ASSERT_TRUE(cursor.next());
  check( 66, 22, 2, pgno_b1, 2, pgno_right, 0, 1); ASSERT_TRUE(cursor.next());
  check( 67, 23, 0, pgno_b1, 3, pgno_right, 0, 1); ASSERT_TRUE(cursor.next());
  check( 68, 23, 1, pgno_b1, 3, pgno_right, 0, 1); ASSERT_TRUE(cursor.next());
  check( 69, 23, 2, pgno_b1, 3, pgno_right, 0, 1); ASSERT_TRUE(cursor.next());

  check( 70, 24, 0, pgno_b2, 0, pgno_right, 1, 1); ASSERT_TRUE(cursor.next());
  check( 71, 24, 1, pgno_b2, 0, pgno_right, 1, 1); ASSERT_TRUE(cursor.next());
  check( 72, 24, 2, pgno_b2, 0, pgno_right, 1, 1); ASSERT_TRUE(cursor.next());
  check( 73, 25, 0, pgno_b2, 1, pgno_right, 1, 1); ASSERT_TRUE(cursor.next());
  check( 74, 25, 1, pgno_b2, 1, pgno_right, 1, 1); ASSERT_TRUE(cursor.next());
  check( 75, 25, 2, pgno_b2, 1, pgno_right, 1, 1); ASSERT_TRUE(cursor.next());
  check( 76, 26, 0, pgno_b2, 2, pgno_right, 1, 1); ASSERT_TRUE(cursor.next());
  check( 77, 26, 1, pgno_b2, 2, pgno_right, 1, 1); ASSERT_TRUE(cursor.next());
  check( 78, 26, 2, pgno_b2, 2, pgno_right, 1, 1); ASSERT_TRUE(cursor.next());
  check( 79, 27, 0, pgno_b2, 3, pgno_right, 1, 1); ASSERT_TRUE(cursor.next());
  check( 80, 27, 1, pgno_b2, 3, pgno_right, 1, 1); ASSERT_TRUE(cursor.next());
  check( 81, 27, 2, pgno_b2, 3, pgno_right, 1, 1); ASSERT_TRUE(cursor.next());
  check( 82, 28, 0, pgno_b2, 4, pgno_right, 1, 1); ASSERT_TRUE(cursor.next());
  check( 83, 28, 1, pgno_b2, 4, pgno_right, 1, 1); ASSERT_TRUE(cursor.next());
  check( 84, 28, 2, pgno_b2, 4, pgno_right, 1, 1); ASSERT_TRUE(cursor.next());
  check( 85, 29, 0, pgno_b2, 5, pgno_right, 1, 1); ASSERT_TRUE(cursor.next());
  check( 86, 29, 1, pgno_b2, 5, pgno_right, 1, 1); ASSERT_TRUE(cursor.next());
  check( 87, 29, 2, pgno_b2, 5, pgno_right, 1, 1); ASSERT_TRUE(cursor.next());

  check( 88, 30, 0, pgno_b3, 0, pgno_right, 2, 1); ASSERT_TRUE(cursor.next());
  check( 89, 30, 1, pgno_b3, 0, pgno_right, 2, 1); ASSERT_TRUE(cursor.next());
  check( 90, 30, 2, pgno_b3, 0, pgno_right, 2, 1); ASSERT_TRUE(cursor.next());
  check( 91, 31, 0, pgno_b3, 1, pgno_right, 2, 1); ASSERT_TRUE(cursor.next());
  check( 92, 31, 1, pgno_b3, 1, pgno_right, 2, 1); ASSERT_TRUE(cursor.next());
  check( 93, 31, 2, pgno_b3, 1, pgno_right, 2, 1); ASSERT_TRUE(cursor.next());
  check( 94, 32, 0, pgno_b3, 2, pgno_right, 2, 1); ASSERT_TRUE(cursor.next());
  check( 95, 32, 1, pgno_b3, 2, pgno_right, 2, 1); ASSERT_TRUE(cursor.next());
  check( 96, 32, 2, pgno_b3, 2, pgno_right, 2, 1); ASSERT_TRUE(cursor.next());
  check( 97, 33, 0, pgno_b3, 3, pgno_right, 2, 1); ASSERT_TRUE(cursor.next());
  check( 98, 33, 1, pgno_b3, 3, pgno_right, 2, 1); ASSERT_TRUE(cursor.next());
  check( 99, 33, 2, pgno_b3, 3, pgno_right, 2, 1); ASSERT_TRUE(cursor.next());
  check(100, 34, 0, pgno_b3, 4, pgno_right, 2, 1); ASSERT_TRUE(cursor.next());
  check(101, 34, 1, pgno_b3, 4, pgno_right, 2, 1); ASSERT_TRUE(cursor.next());
  check(102, 34, 2, pgno_b3, 4, pgno_right, 2, 1); ASSERT_TRUE(cursor.next());

  check(103, 35, 0, pgno_b4, 0, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(104, 35, 1, pgno_b4, 0, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(105, 35, 2, pgno_b4, 0, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(106, 36, 0, pgno_b4, 1, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(107, 36, 1, pgno_b4, 1, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(108, 36, 2, pgno_b4, 1, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(109, 37, 0, pgno_b4, 2, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(110, 37, 1, pgno_b4, 2, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(111, 37, 2, pgno_b4, 2, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(112, 38, 0, pgno_b4, 3, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(113, 38, 1, pgno_b4, 3, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(114, 38, 2, pgno_b4, 3, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(115, 39, 0, pgno_b4, 4, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(116, 39, 1, pgno_b4, 4, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(117, 39, 2, pgno_b4, 4, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(118, 40, 0, pgno_b4, 5, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(119, 40, 1, pgno_b4, 5, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(120, 40, 2, pgno_b4, 5, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(121, 41, 0, pgno_b4, 6, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(122, 41, 1, pgno_b4, 6, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());
  check(123, 41, 2, pgno_b4, 6, pgno_right, 3, 1); ASSERT_TRUE(cursor.next());

  check(124, 42, 0, pgno_b5, 0, pgno_right, 4, 1); ASSERT_TRUE(cursor.next());
  check(125, 42, 1, pgno_b5, 0, pgno_right, 4, 1); ASSERT_TRUE(cursor.next());
  check(126, 42, 2, pgno_b5, 0, pgno_right, 4, 1); ASSERT_TRUE(cursor.next());
  check(127, 43, 0, pgno_b5, 1, pgno_right, 4, 1); ASSERT_TRUE(cursor.next());
  check(128, 43, 1, pgno_b5, 1, pgno_right, 4, 1); ASSERT_TRUE(cursor.next());
  check(129, 43, 2, pgno_b5, 1, pgno_right, 4, 1); ASSERT_TRUE(cursor.next());
  check(130, 44, 0, pgno_b5, 2, pgno_right, 4, 1); ASSERT_TRUE(cursor.next());
  check(131, 44, 1, pgno_b5, 2, pgno_right, 4, 1); ASSERT_TRUE(cursor.next());
  check(132, 44, 2, pgno_b5, 2, pgno_right, 4, 1); ASSERT_TRUE(cursor.next());
  check(133, 45, 0, pgno_b5, 3, pgno_right, 4, 1); ASSERT_TRUE(cursor.next());
  check(134, 45, 1, pgno_b5, 3, pgno_right, 4, 1); ASSERT_TRUE(cursor.next());
  check(135, 45, 2, pgno_b5, 3, pgno_right, 4, 1);
  ASSERT_FALSE(cursor.next());
}

TEST_F(BTreeCursorTest, BTreePrevDeep2) {
  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 3;
  config.node_max_cells = config.leaf_max_cells = 6;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/deep2.yaml",
    pager.get(),
    config
  );

  BTreeCursor cursor(pager.get(), root_pgno, config);

  PageNumber leaf_pgno[46];
  BTreeCursorStack s;

  cursor.move_to_key(1);  s = cursor.get_cursor_stack(); leaf_pgno[1] = s.top().first; s.pop(); PageNumber pgno_a1    = s.top().first; s.pop(); PageNumber pgno_left  = s.top().first;
  cursor.move_to_key(4);  s = cursor.get_cursor_stack(); leaf_pgno[2]  = s.top().first;
  cursor.move_to_key(7);  s = cursor.get_cursor_stack(); leaf_pgno[3]  = s.top().first;
  cursor.move_to_key(10); s = cursor.get_cursor_stack(); leaf_pgno[4]  = s.top().first;

  cursor.move_to_key(13); s = cursor.get_cursor_stack(); leaf_pgno[5]  = s.top().first; s.pop(); PageNumber pgno_a2 = s.top().first;
  cursor.move_to_key(16); s = cursor.get_cursor_stack(); leaf_pgno[6]  = s.top().first;
  cursor.move_to_key(19); s = cursor.get_cursor_stack(); leaf_pgno[7]  = s.top().first;
  cursor.move_to_key(22); s = cursor.get_cursor_stack(); leaf_pgno[8]  = s.top().first;
  cursor.move_to_key(25); s = cursor.get_cursor_stack(); leaf_pgno[9]  = s.top().first;

  cursor.move_to_key(28); s = cursor.get_cursor_stack(); leaf_pgno[10] = s.top().first; s.pop(); PageNumber pgno_a3 = s.top().first;
  cursor.move_to_key(31); s = cursor.get_cursor_stack(); leaf_pgno[11] = s.top().first;
  cursor.move_to_key(34); s = cursor.get_cursor_stack(); leaf_pgno[12] = s.top().first;
  cursor.move_to_key(37); s = cursor.get_cursor_stack(); leaf_pgno[13] = s.top().first;
  cursor.move_to_key(40); s = cursor.get_cursor_stack(); leaf_pgno[14] = s.top().first;
  cursor.move_to_key(43); s = cursor.get_cursor_stack(); leaf_pgno[15] = s.top().first;

  cursor.move_to_key(46); s = cursor.get_cursor_stack(); leaf_pgno[16] = s.top().first; s.pop(); PageNumber pgno_a4 = s.top().first;
  cursor.move_to_key(49); s = cursor.get_cursor_stack(); leaf_pgno[17] = s.top().first;
  cursor.move_to_key(52); s = cursor.get_cursor_stack(); leaf_pgno[18] = s.top().first;
  cursor.move_to_key(55); s = cursor.get_cursor_stack(); leaf_pgno[19] = s.top().first;

  cursor.move_to_key(58); s = cursor.get_cursor_stack(); leaf_pgno[20] = s.top().first; s.pop(); PageNumber pgno_b1 = s.top().first; s.pop(); PageNumber pgno_right = s.top().first;
  cursor.move_to_key(61); s = cursor.get_cursor_stack(); leaf_pgno[21] = s.top().first;
  cursor.move_to_key(64); s = cursor.get_cursor_stack(); leaf_pgno[22] = s.top().first;
  cursor.move_to_key(67); s = cursor.get_cursor_stack(); leaf_pgno[23] = s.top().first;

  cursor.move_to_key(70); s = cursor.get_cursor_stack(); leaf_pgno[24] = s.top().first; s.pop(); PageNumber pgno_b2 = s.top().first;
  cursor.move_to_key(73); s = cursor.get_cursor_stack(); leaf_pgno[25] = s.top().first;
  cursor.move_to_key(76); s = cursor.get_cursor_stack(); leaf_pgno[26] = s.top().first;
  cursor.move_to_key(79); s = cursor.get_cursor_stack(); leaf_pgno[27] = s.top().first;
  cursor.move_to_key(82); s = cursor.get_cursor_stack(); leaf_pgno[28] = s.top().first;
  cursor.move_to_key(85); s = cursor.get_cursor_stack(); leaf_pgno[29] = s.top().first;

  cursor.move_to_key(88);  s = cursor.get_cursor_stack(); leaf_pgno[30] = s.top().first; s.pop(); PageNumber pgno_b3 = s.top().first;
  cursor.move_to_key(91);  s = cursor.get_cursor_stack(); leaf_pgno[31] = s.top().first;
  cursor.move_to_key(94);  s = cursor.get_cursor_stack(); leaf_pgno[32] = s.top().first;
  cursor.move_to_key(97);  s = cursor.get_cursor_stack(); leaf_pgno[33] = s.top().first;
  cursor.move_to_key(100); s = cursor.get_cursor_stack(); leaf_pgno[34] = s.top().first;

  cursor.move_to_key(103); s = cursor.get_cursor_stack(); leaf_pgno[35] = s.top().first; s.pop(); PageNumber pgno_b4 = s.top().first;
  cursor.move_to_key(106); s = cursor.get_cursor_stack(); leaf_pgno[36] = s.top().first;
  cursor.move_to_key(109); s = cursor.get_cursor_stack(); leaf_pgno[37] = s.top().first;
  cursor.move_to_key(112); s = cursor.get_cursor_stack(); leaf_pgno[38] = s.top().first;
  cursor.move_to_key(115); s = cursor.get_cursor_stack(); leaf_pgno[39] = s.top().first;
  cursor.move_to_key(118); s = cursor.get_cursor_stack(); leaf_pgno[40] = s.top().first;
  cursor.move_to_key(121); s = cursor.get_cursor_stack(); leaf_pgno[41] = s.top().first;

  cursor.move_to_key(124); s = cursor.get_cursor_stack(); leaf_pgno[42] = s.top().first; s.pop(); PageNumber pgno_b5 = s.top().first;
  cursor.move_to_key(127); s = cursor.get_cursor_stack(); leaf_pgno[43] = s.top().first;
  cursor.move_to_key(130); s = cursor.get_cursor_stack(); leaf_pgno[44] = s.top().first;
  cursor.move_to_key(133); s = cursor.get_cursor_stack(); leaf_pgno[45] = s.top().first;

  auto check = [&](int key, int ln, size_t lci,
                   PageNumber l3p, size_t l3i,
                   PageNumber l2p, size_t l2i,
                   size_t ri) {
    ASSERT_EQ(cursor.current_key(), key);
    assert_payload(cursor.current_record_pgno(), pager->db_file_ptr_, std::to_string(key));
    assert_cursor_stack(cursor.get_cursor_stack(), {
      {leaf_pgno[ln], lci}, {l3p, l3i}, {l2p, l2i}, {root_pgno, ri}
    });
  };

  cursor.move_to_last();

  check(135, 45, 2, pgno_b5, 3, pgno_right, 4, 1); ASSERT_TRUE(cursor.prev());
  check(134, 45, 1, pgno_b5, 3, pgno_right, 4, 1); ASSERT_TRUE(cursor.prev());
  check(133, 45, 0, pgno_b5, 3, pgno_right, 4, 1); ASSERT_TRUE(cursor.prev());
  check(132, 44, 2, pgno_b5, 2, pgno_right, 4, 1); ASSERT_TRUE(cursor.prev());
  check(131, 44, 1, pgno_b5, 2, pgno_right, 4, 1); ASSERT_TRUE(cursor.prev());
  check(130, 44, 0, pgno_b5, 2, pgno_right, 4, 1); ASSERT_TRUE(cursor.prev());
  check(129, 43, 2, pgno_b5, 1, pgno_right, 4, 1); ASSERT_TRUE(cursor.prev());
  check(128, 43, 1, pgno_b5, 1, pgno_right, 4, 1); ASSERT_TRUE(cursor.prev());
  check(127, 43, 0, pgno_b5, 1, pgno_right, 4, 1); ASSERT_TRUE(cursor.prev());
  check(126, 42, 2, pgno_b5, 0, pgno_right, 4, 1); ASSERT_TRUE(cursor.prev());
  check(125, 42, 1, pgno_b5, 0, pgno_right, 4, 1); ASSERT_TRUE(cursor.prev());
  check(124, 42, 0, pgno_b5, 0, pgno_right, 4, 1); ASSERT_TRUE(cursor.prev());

  check(123, 41, 2, pgno_b4, 6, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(122, 41, 1, pgno_b4, 6, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(121, 41, 0, pgno_b4, 6, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(120, 40, 2, pgno_b4, 5, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(119, 40, 1, pgno_b4, 5, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(118, 40, 0, pgno_b4, 5, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(117, 39, 2, pgno_b4, 4, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(116, 39, 1, pgno_b4, 4, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(115, 39, 0, pgno_b4, 4, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(114, 38, 2, pgno_b4, 3, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(113, 38, 1, pgno_b4, 3, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(112, 38, 0, pgno_b4, 3, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(111, 37, 2, pgno_b4, 2, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(110, 37, 1, pgno_b4, 2, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(109, 37, 0, pgno_b4, 2, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(108, 36, 2, pgno_b4, 1, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(107, 36, 1, pgno_b4, 1, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(106, 36, 0, pgno_b4, 1, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(105, 35, 2, pgno_b4, 0, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(104, 35, 1, pgno_b4, 0, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());
  check(103, 35, 0, pgno_b4, 0, pgno_right, 3, 1); ASSERT_TRUE(cursor.prev());

  check(102, 34, 2, pgno_b3, 4, pgno_right, 2, 1); ASSERT_TRUE(cursor.prev());
  check(101, 34, 1, pgno_b3, 4, pgno_right, 2, 1); ASSERT_TRUE(cursor.prev());
  check(100, 34, 0, pgno_b3, 4, pgno_right, 2, 1); ASSERT_TRUE(cursor.prev());
  check( 99, 33, 2, pgno_b3, 3, pgno_right, 2, 1); ASSERT_TRUE(cursor.prev());
  check( 98, 33, 1, pgno_b3, 3, pgno_right, 2, 1); ASSERT_TRUE(cursor.prev());
  check( 97, 33, 0, pgno_b3, 3, pgno_right, 2, 1); ASSERT_TRUE(cursor.prev());
  check( 96, 32, 2, pgno_b3, 2, pgno_right, 2, 1); ASSERT_TRUE(cursor.prev());
  check( 95, 32, 1, pgno_b3, 2, pgno_right, 2, 1); ASSERT_TRUE(cursor.prev());
  check( 94, 32, 0, pgno_b3, 2, pgno_right, 2, 1); ASSERT_TRUE(cursor.prev());
  check( 93, 31, 2, pgno_b3, 1, pgno_right, 2, 1); ASSERT_TRUE(cursor.prev());
  check( 92, 31, 1, pgno_b3, 1, pgno_right, 2, 1); ASSERT_TRUE(cursor.prev());
  check( 91, 31, 0, pgno_b3, 1, pgno_right, 2, 1); ASSERT_TRUE(cursor.prev());
  check( 90, 30, 2, pgno_b3, 0, pgno_right, 2, 1); ASSERT_TRUE(cursor.prev());
  check( 89, 30, 1, pgno_b3, 0, pgno_right, 2, 1); ASSERT_TRUE(cursor.prev());
  check( 88, 30, 0, pgno_b3, 0, pgno_right, 2, 1); ASSERT_TRUE(cursor.prev());

  check( 87, 29, 2, pgno_b2, 5, pgno_right, 1, 1); ASSERT_TRUE(cursor.prev());
  check( 86, 29, 1, pgno_b2, 5, pgno_right, 1, 1); ASSERT_TRUE(cursor.prev());
  check( 85, 29, 0, pgno_b2, 5, pgno_right, 1, 1); ASSERT_TRUE(cursor.prev());
  check( 84, 28, 2, pgno_b2, 4, pgno_right, 1, 1); ASSERT_TRUE(cursor.prev());
  check( 83, 28, 1, pgno_b2, 4, pgno_right, 1, 1); ASSERT_TRUE(cursor.prev());
  check( 82, 28, 0, pgno_b2, 4, pgno_right, 1, 1); ASSERT_TRUE(cursor.prev());
  check( 81, 27, 2, pgno_b2, 3, pgno_right, 1, 1); ASSERT_TRUE(cursor.prev());
  check( 80, 27, 1, pgno_b2, 3, pgno_right, 1, 1); ASSERT_TRUE(cursor.prev());
  check( 79, 27, 0, pgno_b2, 3, pgno_right, 1, 1); ASSERT_TRUE(cursor.prev());
  check( 78, 26, 2, pgno_b2, 2, pgno_right, 1, 1); ASSERT_TRUE(cursor.prev());
  check( 77, 26, 1, pgno_b2, 2, pgno_right, 1, 1); ASSERT_TRUE(cursor.prev());
  check( 76, 26, 0, pgno_b2, 2, pgno_right, 1, 1); ASSERT_TRUE(cursor.prev());
  check( 75, 25, 2, pgno_b2, 1, pgno_right, 1, 1); ASSERT_TRUE(cursor.prev());
  check( 74, 25, 1, pgno_b2, 1, pgno_right, 1, 1); ASSERT_TRUE(cursor.prev());
  check( 73, 25, 0, pgno_b2, 1, pgno_right, 1, 1); ASSERT_TRUE(cursor.prev());
  check( 72, 24, 2, pgno_b2, 0, pgno_right, 1, 1); ASSERT_TRUE(cursor.prev());
  check( 71, 24, 1, pgno_b2, 0, pgno_right, 1, 1); ASSERT_TRUE(cursor.prev());
  check( 70, 24, 0, pgno_b2, 0, pgno_right, 1, 1); ASSERT_TRUE(cursor.prev());

  check( 69, 23, 2, pgno_b1, 3, pgno_right, 0, 1); ASSERT_TRUE(cursor.prev());
  check( 68, 23, 1, pgno_b1, 3, pgno_right, 0, 1); ASSERT_TRUE(cursor.prev());
  check( 67, 23, 0, pgno_b1, 3, pgno_right, 0, 1); ASSERT_TRUE(cursor.prev());
  check( 66, 22, 2, pgno_b1, 2, pgno_right, 0, 1); ASSERT_TRUE(cursor.prev());
  check( 65, 22, 1, pgno_b1, 2, pgno_right, 0, 1); ASSERT_TRUE(cursor.prev());
  check( 64, 22, 0, pgno_b1, 2, pgno_right, 0, 1); ASSERT_TRUE(cursor.prev());
  check( 63, 21, 2, pgno_b1, 1, pgno_right, 0, 1); ASSERT_TRUE(cursor.prev());
  check( 62, 21, 1, pgno_b1, 1, pgno_right, 0, 1); ASSERT_TRUE(cursor.prev());
  check( 61, 21, 0, pgno_b1, 1, pgno_right, 0, 1); ASSERT_TRUE(cursor.prev());
  check( 60, 20, 2, pgno_b1, 0, pgno_right, 0, 1); ASSERT_TRUE(cursor.prev());
  check( 59, 20, 1, pgno_b1, 0, pgno_right, 0, 1); ASSERT_TRUE(cursor.prev());
  check( 58, 20, 0, pgno_b1, 0, pgno_right, 0, 1); ASSERT_TRUE(cursor.prev());

  check( 57, 19, 2, pgno_a4, 3, pgno_left, 3, 0); ASSERT_TRUE(cursor.prev());
  check( 56, 19, 1, pgno_a4, 3, pgno_left, 3, 0); ASSERT_TRUE(cursor.prev());
  check( 55, 19, 0, pgno_a4, 3, pgno_left, 3, 0); ASSERT_TRUE(cursor.prev());
  check( 54, 18, 2, pgno_a4, 2, pgno_left, 3, 0); ASSERT_TRUE(cursor.prev());
  check( 53, 18, 1, pgno_a4, 2, pgno_left, 3, 0); ASSERT_TRUE(cursor.prev());
  check( 52, 18, 0, pgno_a4, 2, pgno_left, 3, 0); ASSERT_TRUE(cursor.prev());
  check( 51, 17, 2, pgno_a4, 1, pgno_left, 3, 0); ASSERT_TRUE(cursor.prev());
  check( 50, 17, 1, pgno_a4, 1, pgno_left, 3, 0); ASSERT_TRUE(cursor.prev());
  check( 49, 17, 0, pgno_a4, 1, pgno_left, 3, 0); ASSERT_TRUE(cursor.prev());
  check( 48, 16, 2, pgno_a4, 0, pgno_left, 3, 0); ASSERT_TRUE(cursor.prev());
  check( 47, 16, 1, pgno_a4, 0, pgno_left, 3, 0); ASSERT_TRUE(cursor.prev());
  check( 46, 16, 0, pgno_a4, 0, pgno_left, 3, 0); ASSERT_TRUE(cursor.prev());

  check( 45, 15, 2, pgno_a3, 5, pgno_left, 2, 0); ASSERT_TRUE(cursor.prev());
  check( 44, 15, 1, pgno_a3, 5, pgno_left, 2, 0); ASSERT_TRUE(cursor.prev());
  check( 43, 15, 0, pgno_a3, 5, pgno_left, 2, 0); ASSERT_TRUE(cursor.prev());
  check( 42, 14, 2, pgno_a3, 4, pgno_left, 2, 0); ASSERT_TRUE(cursor.prev());
  check( 41, 14, 1, pgno_a3, 4, pgno_left, 2, 0); ASSERT_TRUE(cursor.prev());
  check( 40, 14, 0, pgno_a3, 4, pgno_left, 2, 0); ASSERT_TRUE(cursor.prev());
  check( 39, 13, 2, pgno_a3, 3, pgno_left, 2, 0); ASSERT_TRUE(cursor.prev());
  check( 38, 13, 1, pgno_a3, 3, pgno_left, 2, 0); ASSERT_TRUE(cursor.prev());
  check( 37, 13, 0, pgno_a3, 3, pgno_left, 2, 0); ASSERT_TRUE(cursor.prev());
  check( 36, 12, 2, pgno_a3, 2, pgno_left, 2, 0); ASSERT_TRUE(cursor.prev());
  check( 35, 12, 1, pgno_a3, 2, pgno_left, 2, 0); ASSERT_TRUE(cursor.prev());
  check( 34, 12, 0, pgno_a3, 2, pgno_left, 2, 0); ASSERT_TRUE(cursor.prev());
  check( 33, 11, 2, pgno_a3, 1, pgno_left, 2, 0); ASSERT_TRUE(cursor.prev());
  check( 32, 11, 1, pgno_a3, 1, pgno_left, 2, 0); ASSERT_TRUE(cursor.prev());
  check( 31, 11, 0, pgno_a3, 1, pgno_left, 2, 0); ASSERT_TRUE(cursor.prev());
  check( 30, 10, 2, pgno_a3, 0, pgno_left, 2, 0); ASSERT_TRUE(cursor.prev());
  check( 29, 10, 1, pgno_a3, 0, pgno_left, 2, 0); ASSERT_TRUE(cursor.prev());
  check( 28, 10, 0, pgno_a3, 0, pgno_left, 2, 0); ASSERT_TRUE(cursor.prev());

  check( 27,  9, 2, pgno_a2, 4, pgno_left, 1, 0); ASSERT_TRUE(cursor.prev());
  check( 26,  9, 1, pgno_a2, 4, pgno_left, 1, 0); ASSERT_TRUE(cursor.prev());
  check( 25,  9, 0, pgno_a2, 4, pgno_left, 1, 0); ASSERT_TRUE(cursor.prev());
  check( 24,  8, 2, pgno_a2, 3, pgno_left, 1, 0); ASSERT_TRUE(cursor.prev());
  check( 23,  8, 1, pgno_a2, 3, pgno_left, 1, 0); ASSERT_TRUE(cursor.prev());
  check( 22,  8, 0, pgno_a2, 3, pgno_left, 1, 0); ASSERT_TRUE(cursor.prev());
  check( 21,  7, 2, pgno_a2, 2, pgno_left, 1, 0); ASSERT_TRUE(cursor.prev());
  check( 20,  7, 1, pgno_a2, 2, pgno_left, 1, 0); ASSERT_TRUE(cursor.prev());
  check( 19,  7, 0, pgno_a2, 2, pgno_left, 1, 0); ASSERT_TRUE(cursor.prev());
  check( 18,  6, 2, pgno_a2, 1, pgno_left, 1, 0); ASSERT_TRUE(cursor.prev());
  check( 17,  6, 1, pgno_a2, 1, pgno_left, 1, 0); ASSERT_TRUE(cursor.prev());
  check( 16,  6, 0, pgno_a2, 1, pgno_left, 1, 0); ASSERT_TRUE(cursor.prev());
  check( 15,  5, 2, pgno_a2, 0, pgno_left, 1, 0); ASSERT_TRUE(cursor.prev());
  check( 14,  5, 1, pgno_a2, 0, pgno_left, 1, 0); ASSERT_TRUE(cursor.prev());
  check( 13,  5, 0, pgno_a2, 0, pgno_left, 1, 0); ASSERT_TRUE(cursor.prev());

  check( 12,  4, 2, pgno_a1, 3, pgno_left, 0, 0); ASSERT_TRUE(cursor.prev());
  check( 11,  4, 1, pgno_a1, 3, pgno_left, 0, 0); ASSERT_TRUE(cursor.prev());
  check( 10,  4, 0, pgno_a1, 3, pgno_left, 0, 0); ASSERT_TRUE(cursor.prev());
  check(  9,  3, 2, pgno_a1, 2, pgno_left, 0, 0); ASSERT_TRUE(cursor.prev());
  check(  8,  3, 1, pgno_a1, 2, pgno_left, 0, 0); ASSERT_TRUE(cursor.prev());
  check(  7,  3, 0, pgno_a1, 2, pgno_left, 0, 0); ASSERT_TRUE(cursor.prev());
  check(  6,  2, 2, pgno_a1, 1, pgno_left, 0, 0); ASSERT_TRUE(cursor.prev());
  check(  5,  2, 1, pgno_a1, 1, pgno_left, 0, 0); ASSERT_TRUE(cursor.prev());
  check(  4,  2, 0, pgno_a1, 1, pgno_left, 0, 0); ASSERT_TRUE(cursor.prev());
  check(  3,  1, 2, pgno_a1, 0, pgno_left, 0, 0); ASSERT_TRUE(cursor.prev());
  check(  2,  1, 1, pgno_a1, 0, pgno_left, 0, 0); ASSERT_TRUE(cursor.prev());
  check(  1,  1, 0, pgno_a1, 0, pgno_left, 0, 0);
  ASSERT_FALSE(cursor.prev());
}