#include <cstddef>
#include <string>

#include <gtest/gtest.h>
#include "db_test_fixture.h"
#include "btree/btree.h"
#include "btree/builder/btree_builder.h"
#include "pager/leaf_page/leaf_page.h"

class BTreeCursorInsertTest : public DbTestFixture {};

TEST_F(BTreeCursorInsertTest, BTreeInsertBasic1) {
  ASSERT_NE(db_file_ptr, nullptr);
  ASSERT_NE(pager, nullptr);

  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 2;
  config.node_max_cells = config.leaf_max_cells = 3;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/basic1.yaml",
    pager.get(),
    config
  );

  BTreeCursor cursor(pager.get(), root_pgno, config);

  std::vector<std::byte> payload1 = generate_random_payload(1024);
  cursor.insert(6, payload1);
  ASSERT_EQ(cursor.current_key(), 6);

  NodePageManager npm(root_pgno, db_file_ptr);
  ASSERT_EQ(npm.num_cells_, 3);
  ASSERT_EQ(npm.cells_[0].key, 4);
  ASSERT_EQ(npm.cells_[1].key, 9);
  ASSERT_EQ(npm.cells_[2].key, 12);

  LeafPageManager lpm1(npm.cells_[0].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(lpm1.num_cells_, 2);
  ASSERT_EQ(lpm1.cells_[0].key, 1);
  ASSERT_EQ(lpm1.cells_[1].key, 3);

  LeafPageManager lpm2(npm.cells_[1].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(lpm2.num_cells_, 2);
  ASSERT_EQ(lpm2.cells_[0].key, 5);
  ASSERT_EQ(lpm2.cells_[1].key, 6);

  LeafPageManager lpm3(npm.cells_[2].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(lpm3.num_cells_, 2);
  ASSERT_EQ(lpm3.cells_[0].key, 9);
  ASSERT_EQ(lpm3.cells_[1].key, 10);

  LeafPageManager lpm4(npm.right_child_, db_file_ptr, pager.get());
  ASSERT_EQ(lpm4.num_cells_, 2);
  ASSERT_EQ(lpm4.cells_[0].key, 12);
  ASSERT_EQ(lpm4.cells_[1].key, 13);

  ASSERT_EQ(lpm1.prev_page(), NULL_PAGE);
  ASSERT_EQ(lpm1.next_page(), lpm2.pgno_);

  ASSERT_EQ(lpm2.prev_page(), lpm1.pgno_);
  ASSERT_EQ(lpm2.next_page(), lpm3.pgno_);

  ASSERT_EQ(lpm3.prev_page(), lpm2.pgno_);
  ASSERT_EQ(lpm3.next_page(), lpm4.pgno_);

  ASSERT_EQ(lpm4.prev_page(), lpm3.pgno_);
  ASSERT_EQ(lpm4.next_page(), NULL_PAGE);

  std::vector<std::byte> payload2 = generate_random_payload(1024);
  cursor.insert(8, payload2);
  ASSERT_EQ(cursor.current_key(), 8);

  lpm2 = LeafPageManager(npm.cells_[1].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(lpm2.num_cells_, 3);
  ASSERT_EQ(lpm2.cells_[0].key, 5);
  ASSERT_EQ(lpm2.cells_[1].key, 6);
  ASSERT_EQ(lpm2.cells_[2].key, 8);
}

TEST_F(BTreeCursorInsertTest, BtreeInsertBasic2) {
  ASSERT_NE(db_file_ptr, nullptr);
  ASSERT_NE(pager, nullptr);

  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 2;
  config.node_max_cells = config.leaf_max_cells = 4;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/basic2.yaml",
    pager.get(),
    config
  );

  BTreeCursor cursor(pager.get(), root_pgno, config);

  std::vector<std::byte> payload1 = generate_random_payload(1024);
  cursor.insert(17, payload1);
  ASSERT_EQ(cursor.current_key(), 17);

  NodePageManager npm(root_pgno, db_file_ptr);
  ASSERT_EQ(npm.num_cells_, 4);
  ASSERT_EQ(npm.cells_[0].key, 5);
  ASSERT_EQ(npm.cells_[1].key, 9);
  ASSERT_EQ(npm.cells_[2].key, 13);
  ASSERT_EQ(npm.cells_[3].key, 19);

  LeafPageManager lpm1(npm.cells_[0].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(lpm1.num_cells_, 2);
  ASSERT_EQ(lpm1.cells_[0].key, 1);
  ASSERT_EQ(lpm1.cells_[1].key, 3);

  LeafPageManager lpm2(npm.cells_[1].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(lpm2.num_cells_, 2);
  ASSERT_EQ(lpm2.cells_[0].key, 5);
  ASSERT_EQ(lpm2.cells_[1].key, 7);

  LeafPageManager lpm3(npm.cells_[2].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(lpm3.num_cells_, 2);
  ASSERT_EQ(lpm3.cells_[0].key, 9);
  ASSERT_EQ(lpm3.cells_[1].key, 11);

  LeafPageManager lpm4(npm.cells_[3].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(lpm4.num_cells_, 4);
  ASSERT_EQ(lpm4.cells_[0].key, 13);
  ASSERT_EQ(lpm4.cells_[1].key, 14);
  ASSERT_EQ(lpm4.cells_[2].key, 15);
  ASSERT_EQ(lpm4.cells_[3].key, 17);

  LeafPageManager lpm5(npm.right_child_, db_file_ptr, pager.get());
  ASSERT_EQ(lpm5.num_cells_, 3);
  ASSERT_EQ(lpm5.cells_[0].key, 20);
  ASSERT_EQ(lpm5.cells_[1].key, 21);
  ASSERT_EQ(lpm5.cells_[2].key, 23);

  ASSERT_EQ(lpm1.prev_page(), NULL_PAGE);
  ASSERT_EQ(lpm1.next_page(), lpm2.pgno_);

  ASSERT_EQ(lpm2.prev_page(), lpm1.pgno_);
  ASSERT_EQ(lpm2.next_page(), lpm3.pgno_);

  ASSERT_EQ(lpm3.prev_page(), lpm2.pgno_);
  ASSERT_EQ(lpm3.next_page(), lpm4.pgno_);

  ASSERT_EQ(lpm4.prev_page(), lpm3.pgno_);
  ASSERT_EQ(lpm4.next_page(), lpm5.pgno_);

  ASSERT_EQ(lpm5.prev_page(), lpm4.pgno_);
  ASSERT_EQ(lpm5.next_page(), NULL_PAGE);

  PageNumber old_root_pgno = cursor.get_root_pgno();
  std::vector<std::byte> payload2 = generate_random_payload(1024);
  cursor.insert(16, payload2);
  ASSERT_EQ(cursor.current_key(), 16);
  PageNumber new_root_pgno = cursor.get_root_pgno();

  ASSERT_NE(new_root_pgno, old_root_pgno);

  NodePageManager new_root(new_root_pgno, db_file_ptr);
  ASSERT_EQ(new_root.num_cells_, 1);
  ASSERT_EQ(new_root.cells_[0].key, 13);

  NodePageManager left_npm(new_root.cells_[0].left_child, db_file_ptr);
  ASSERT_EQ(left_npm.num_cells_, 2);
  ASSERT_EQ(left_npm.cells_[0].key, 5);
  ASSERT_EQ(left_npm.cells_[1].key, 9);

  LeafPageManager l1(left_npm.cells_[0].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(l1.num_cells_, 2);
  ASSERT_EQ(l1.cells_[0].key, 1);
  ASSERT_EQ(l1.cells_[1].key, 3);

  LeafPageManager l2(left_npm.cells_[1].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(l2.num_cells_, 2);
  ASSERT_EQ(l2.cells_[0].key, 5);
  ASSERT_EQ(l2.cells_[1].key, 7);

  LeafPageManager l3(left_npm.right_child_, db_file_ptr, pager.get());
  ASSERT_EQ(l3.num_cells_, 2);
  ASSERT_EQ(l3.cells_[0].key, 9);
  ASSERT_EQ(l3.cells_[1].key, 11);

  NodePageManager right_npm(new_root.right_child_, db_file_ptr);
  ASSERT_EQ(right_npm.num_cells_, 2);
  ASSERT_EQ(right_npm.cells_[0].key, 16);
  ASSERT_EQ(right_npm.cells_[1].key, 19);

  LeafPageManager l4(right_npm.cells_[0].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(l4.num_cells_, 3);
  ASSERT_EQ(l4.cells_[0].key, 13);
  ASSERT_EQ(l4.cells_[1].key, 14);
  ASSERT_EQ(l4.cells_[2].key, 15);

  LeafPageManager l5(right_npm.cells_[1].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(l5.num_cells_, 2);
  ASSERT_EQ(l5.cells_[0].key, 16);
  ASSERT_EQ(l5.cells_[1].key, 17);

  LeafPageManager l6(right_npm.right_child_, db_file_ptr, pager.get());
  ASSERT_EQ(l6.num_cells_, 3);
  ASSERT_EQ(l6.cells_[0].key, 20);
  ASSERT_EQ(l6.cells_[1].key, 21);
  ASSERT_EQ(l6.cells_[2].key, 23);

  ASSERT_EQ(l1.prev_page(), NULL_PAGE);
  ASSERT_EQ(l1.next_page(), l2.pgno_);

  ASSERT_EQ(l2.prev_page(), l1.pgno_);
  ASSERT_EQ(l2.next_page(), l3.pgno_);

  ASSERT_EQ(l3.prev_page(), l2.pgno_);
  ASSERT_EQ(l3.next_page(), l4.pgno_);

  ASSERT_EQ(l4.prev_page(), l3.pgno_);
  ASSERT_EQ(l4.next_page(), l5.pgno_);

  ASSERT_EQ(l5.prev_page(), l4.pgno_);
  ASSERT_EQ(l5.next_page(), l6.pgno_);

  ASSERT_EQ(l6.prev_page(), l5.pgno_);
  ASSERT_EQ(l6.next_page(), NULL_PAGE);
}

TEST_F(BTreeCursorInsertTest, BTreeInsertEmpty1) {
  ASSERT_NE(db_file_ptr, nullptr);
  ASSERT_NE(pager, nullptr);

  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 2;
  config.node_max_cells = config.leaf_max_cells = 4;

  PageNumber root_pgno = pager->create_page(PAGER_LEAF_PAGE);
  BTreeCursor cursor(pager.get(), root_pgno, config);

  for (int i = 1; i <= 25; i++) {
    cursor.insert(i, generate_random_payload(512));
    ASSERT_EQ(cursor.current_key(), i);
  }

  PageNumber new_root_pgno = cursor.get_root_pgno();

  NodePageManager root_npm(new_root_pgno, db_file_ptr);
  ASSERT_EQ(root_npm.num_cells_, 1);
  ASSERT_EQ(root_npm.cells_[0].key, 10);

  NodePageManager left_npm(root_npm.cells_[0].left_child, db_file_ptr);
  ASSERT_EQ(left_npm.num_cells_, 2);
  ASSERT_EQ(left_npm.cells_[0].key, 4);
  ASSERT_EQ(left_npm.cells_[1].key, 7);

  LeafPageManager l1(left_npm.cells_[0].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(l1.num_cells_, 3);
  ASSERT_EQ(l1.cells_[0].key, 1);
  ASSERT_EQ(l1.cells_[1].key, 2);
  ASSERT_EQ(l1.cells_[2].key, 3);

  LeafPageManager l2(left_npm.cells_[1].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(l2.num_cells_, 3);
  ASSERT_EQ(l2.cells_[0].key, 4);
  ASSERT_EQ(l2.cells_[1].key, 5);
  ASSERT_EQ(l2.cells_[2].key, 6);

  LeafPageManager l3(left_npm.right_child_, db_file_ptr, pager.get());
  ASSERT_EQ(l3.num_cells_, 3);
  ASSERT_EQ(l3.cells_[0].key, 7);
  ASSERT_EQ(l3.cells_[1].key, 8);
  ASSERT_EQ(l3.cells_[2].key, 9);

  NodePageManager right_npm(root_npm.right_child_, db_file_ptr);
  ASSERT_EQ(right_npm.num_cells_, 4);
  ASSERT_EQ(right_npm.cells_[0].key, 13);
  ASSERT_EQ(right_npm.cells_[1].key, 16);
  ASSERT_EQ(right_npm.cells_[2].key, 19);
  ASSERT_EQ(right_npm.cells_[3].key, 22);

  LeafPageManager l4(right_npm.cells_[0].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(l4.num_cells_, 3);
  ASSERT_EQ(l4.cells_[0].key, 10);
  ASSERT_EQ(l4.cells_[1].key, 11);
  ASSERT_EQ(l4.cells_[2].key, 12);

  LeafPageManager l5(right_npm.cells_[1].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(l5.num_cells_, 3);
  ASSERT_EQ(l5.cells_[0].key, 13);
  ASSERT_EQ(l5.cells_[1].key, 14);
  ASSERT_EQ(l5.cells_[2].key, 15);

  LeafPageManager l6(right_npm.cells_[2].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(l6.num_cells_, 3);
  ASSERT_EQ(l6.cells_[0].key, 16);
  ASSERT_EQ(l6.cells_[1].key, 17);
  ASSERT_EQ(l6.cells_[2].key, 18);

  LeafPageManager l7(right_npm.cells_[3].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(l7.num_cells_, 3);
  ASSERT_EQ(l7.cells_[0].key, 19);
  ASSERT_EQ(l7.cells_[1].key, 20);
  ASSERT_EQ(l7.cells_[2].key, 21);

  LeafPageManager l8(right_npm.right_child_, db_file_ptr, pager.get());
  ASSERT_EQ(l8.num_cells_, 4);
  ASSERT_EQ(l8.cells_[0].key, 22);
  ASSERT_EQ(l8.cells_[1].key, 23);
  ASSERT_EQ(l8.cells_[2].key, 24);
  ASSERT_EQ(l8.cells_[3].key, 25);

  ASSERT_EQ(l1.prev_page(), NULL_PAGE);
  ASSERT_EQ(l1.next_page(), l2.pgno_);

  ASSERT_EQ(l2.prev_page(), l1.pgno_);
  ASSERT_EQ(l2.next_page(), l3.pgno_);

  ASSERT_EQ(l3.prev_page(), l2.pgno_);
  ASSERT_EQ(l3.next_page(), l4.pgno_);

  ASSERT_EQ(l4.prev_page(), l3.pgno_);
  ASSERT_EQ(l4.next_page(), l5.pgno_);

  ASSERT_EQ(l5.prev_page(), l4.pgno_);
  ASSERT_EQ(l5.next_page(), l6.pgno_);

  ASSERT_EQ(l6.prev_page(), l5.pgno_);
  ASSERT_EQ(l6.next_page(), l7.pgno_);

  ASSERT_EQ(l7.prev_page(), l6.pgno_);
  ASSERT_EQ(l7.next_page(), l8.pgno_);

  ASSERT_EQ(l8.prev_page(), l7.pgno_);
  ASSERT_EQ(l8.next_page(), NULL_PAGE);
}

TEST_F(BTreeCursorInsertTest, BTreeInsertEmpty2) {
  ASSERT_NE(db_file_ptr, nullptr);
  ASSERT_NE(pager, nullptr);

  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 3;
  config.node_max_cells = config.leaf_max_cells = 6;

  PageNumber root_pgno = pager->create_page(PAGER_LEAF_PAGE);
  BTreeCursor cursor(pager.get(), root_pgno, config);

  for (int i = 1; i <= 100; i++) {
    cursor.insert(i, generate_random_payload(512));
    ASSERT_EQ(cursor.current_key(), i);
  }

  NodePageManager root_npm(cursor.get_root_pgno(), db_file_ptr);
  ASSERT_EQ(root_npm.num_cells_, 5);
  ASSERT_EQ(root_npm.cells_[0].key, 17);
  ASSERT_EQ(root_npm.cells_[1].key, 33);
  ASSERT_EQ(root_npm.cells_[2].key, 49);
  ASSERT_EQ(root_npm.cells_[3].key, 65);
  ASSERT_EQ(root_npm.cells_[4].key, 81);

  NodePageManager node_53_57_61(root_npm.cells_[3].left_child, db_file_ptr);
  ASSERT_EQ(node_53_57_61.num_cells_, 3);
  ASSERT_EQ(node_53_57_61.cells_[0].key, 53);
  ASSERT_EQ(node_53_57_61.cells_[1].key, 57);
  ASSERT_EQ(node_53_57_61.cells_[2].key, 61);

  LeafPageManager lc0(node_53_57_61.cells_[0].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(lc0.num_cells_, 4);
  ASSERT_EQ(lc0.cells_[0].key, 49);
  ASSERT_EQ(lc0.cells_[3].key, 52);

  LeafPageManager lc1(node_53_57_61.cells_[1].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(lc1.num_cells_, 4);
  ASSERT_EQ(lc1.cells_[0].key, 53);
  ASSERT_EQ(lc1.cells_[3].key, 56);

  LeafPageManager lc2(node_53_57_61.cells_[2].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(lc2.num_cells_, 4);
  ASSERT_EQ(lc2.cells_[0].key, 57);
  ASSERT_EQ(lc2.cells_[3].key, 60);

  LeafPageManager lc3(node_53_57_61.right_child_, db_file_ptr, pager.get());
  ASSERT_EQ(lc3.num_cells_, 4);
  ASSERT_EQ(lc3.cells_[0].key, 61);
  ASSERT_EQ(lc3.cells_[3].key, 64);

  NodePageManager node_37_41_45(root_npm.cells_[2].left_child, db_file_ptr);
  LeafPageManager leaf_45_48(node_37_41_45.right_child_, db_file_ptr, pager.get());
  ASSERT_EQ(leaf_45_48.num_cells_, 4);
  ASSERT_EQ(leaf_45_48.cells_[0].key, 45);
  ASSERT_EQ(leaf_45_48.cells_[1].key, 46);
  ASSERT_EQ(leaf_45_48.cells_[2].key, 47);
  ASSERT_EQ(leaf_45_48.cells_[3].key, 48);

  LeafPageManager prev_leaf(node_37_41_45.cells_[2].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(prev_leaf.cells_[0].key, 41);
  ASSERT_EQ(prev_leaf.cells_[3].key, 44);
  ASSERT_EQ(leaf_45_48.prev_page(), prev_leaf.pgno_);
  ASSERT_EQ(leaf_45_48.next_page(), lc0.pgno_);
}

TEST_F(BTreeCursorInsertTest, BTreeInsertReverseSorted) {
  ASSERT_NE(db_file_ptr, nullptr);
  ASSERT_NE(pager, nullptr);

  BTreeConfig config;
  config.node_min_cells = config.leaf_min_cells = 2;
  config.node_max_cells = config.leaf_max_cells = 4;

  PageNumber root_pgno = pager->create_page(PAGER_LEAF_PAGE);
  BTreeCursor cursor(pager.get(), root_pgno, config);

  for (int i = 10; i >= 1; i--) {
    cursor.insert(i, generate_random_payload(512));
    ASSERT_EQ(cursor.current_key(), i);
  }

  auto assert_sorted = [](std::vector<DefaultPagerKey> keys) {
    for (size_t i = 1; i < keys.size(); i++)
      ASSERT_LT(keys[i-1], keys[i]);
  };

  NodePageManager root_npm(cursor.get_root_pgno(), db_file_ptr);
  ASSERT_EQ(root_npm.num_cells_, 3);

  std::vector<DefaultPagerKey> root_keys;
  for (size_t i = 0; i < root_npm.num_cells_; i++)
    root_keys.push_back(root_npm.cells_[i].key);
  assert_sorted(root_keys);

  ASSERT_EQ(root_npm.cells_[0].key, 5);
  ASSERT_EQ(root_npm.cells_[1].key, 7);
  ASSERT_EQ(root_npm.cells_[2].key, 9);

  LeafPageManager lpm0(root_npm.cells_[0].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(lpm0.num_cells_, 4);
  assert_sorted({lpm0.cells_[0].key, lpm0.cells_[1].key, lpm0.cells_[2].key, lpm0.cells_[3].key});
  ASSERT_EQ(lpm0.cells_[0].key, 1);
  ASSERT_EQ(lpm0.cells_[3].key, 4);

  LeafPageManager lpm1(root_npm.cells_[1].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(lpm1.num_cells_, 2);
  assert_sorted({lpm1.cells_[0].key, lpm1.cells_[1].key});
  ASSERT_EQ(lpm1.cells_[0].key, 5);
  ASSERT_EQ(lpm1.cells_[1].key, 6);

  LeafPageManager lpm2(root_npm.cells_[2].left_child, db_file_ptr, pager.get());
  ASSERT_EQ(lpm2.num_cells_, 2);
  assert_sorted({lpm2.cells_[0].key, lpm2.cells_[1].key});
  ASSERT_EQ(lpm2.cells_[0].key, 7);
  ASSERT_EQ(lpm2.cells_[1].key, 8);

  LeafPageManager lpm3(root_npm.right_child_, db_file_ptr, pager.get());
  ASSERT_EQ(lpm3.num_cells_, 2);
  assert_sorted({lpm3.cells_[0].key, lpm3.cells_[1].key});
  ASSERT_EQ(lpm3.cells_[0].key, 9);
  ASSERT_EQ(lpm3.cells_[1].key, 10);
}