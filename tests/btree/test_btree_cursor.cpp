#include <cstddef>

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
  BTreeCursorStackElt curr = stack.top();

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
  BTreeCursorStackElt curr = stack.top();

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