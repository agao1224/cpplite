#include <cstddef>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "shared.h"
#include "db_test_fixture.h"
#include "btree/btree_test_utils.h"
#include "btree/builder/btree_builder.h"

static void assert_leaf_cells(
  const LeafPageManager& lpm,
  const std::vector<DefaultPagerKey>& expected_keys,
  const std::vector<size_t>& expected_sizes,
  const std::vector<std::string>& expected_payloads,
  std::shared_ptr<OsFile> db_file_ptr
) {
  ASSERT_EQ(lpm.cells_.size(), expected_keys.size());
  ASSERT_EQ(lpm.num_cells_, expected_keys.size());
  for (size_t i = 0; i < expected_keys.size(); i++) {
    EXPECT_EQ(lpm.cells_[i].key, expected_keys[i]);
    EXPECT_EQ(lpm.cells_[i].payload_size, expected_sizes[i]);
    assert_payload(lpm.cells_[i].record_page, db_file_ptr, expected_payloads[i]);
  }
}

static void assert_leaf_page(
  PageNumber pgno,
  Pager* pager,
  std::shared_ptr<OsFile> db_file_ptr,
  const std::vector<DefaultPagerKey>& expected_keys,
  const std::vector<size_t>& expected_sizes,
  const std::vector<std::string>& expected_payloads
) {
  ASSERT_EQ(pager->get_page_type(pgno), PAGER_LEAF_PAGE);
  LeafPageManager lpm(pgno, db_file_ptr, pager);
  assert_leaf_cells(lpm, expected_keys, expected_sizes, expected_payloads, db_file_ptr);
}


class BTreeBuilderTest : public DbTestFixture {};

TEST_F(BTreeBuilderTest, BTreeBuildBasic1) {
  ASSERT_NE(db_file_ptr, nullptr);
  ASSERT_NE(pager, nullptr);

  BTreeConfig config;
  config.leaf_min_cells = 0;
  config.node_min_cells = 0;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/basic1.yaml",
    pager.get(),
    config
  );

  ASSERT_EQ(pager->get_page_type(root_pgno), PAGER_NODE_PAGE);

  NodePageManager npm(root_pgno, db_file_ptr);
  ASSERT_TRUE(npm.cells_.size() == 2 && npm.cells_.size() == npm.num_cells_);
  ASSERT_EQ(npm.cells_[0].key, 4);
  ASSERT_EQ(npm.cells_[1].key, 12);
  ASSERT_NE(npm.cells_[0].left_child, NULL_PAGE);
  ASSERT_NE(npm.cells_[1].left_child, NULL_PAGE);
  ASSERT_NE(npm.right_child_, NULL_PAGE);

  assert_leaf_page(
    npm.cells_[0].left_child, pager.get(), db_file_ptr,
    {1, 3},
    {sizeof("111") - 1, sizeof("333") - 1},
    {"111", "333"}
  );

  assert_leaf_page(
    npm.cells_[1].left_child, pager.get(), db_file_ptr,
    {5, 9, 10},
    {sizeof("555") - 1, sizeof("999") - 1, sizeof("101010") - 1},
    {"555", "999", "101010"}
  );

  assert_leaf_page(
    npm.right_child_, pager.get(), db_file_ptr,
    {12, 13},
    {sizeof("121212") - 1, sizeof("131313") - 1},
    {"121212", "131313"}
  );
}

TEST_F(BTreeBuilderTest, BTreeBuildBasic2) {
  ASSERT_NE(db_file_ptr, nullptr);
  ASSERT_NE(pager, nullptr);

  BTreeConfig config;
  config.leaf_min_cells = 0;
  config.node_min_cells = 0;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/basic2.yaml",
    pager.get(),
    config
  );

  ASSERT_EQ(pager->get_page_type(root_pgno), PAGER_NODE_PAGE);

  NodePageManager npm(root_pgno, db_file_ptr);
  ASSERT_TRUE(npm.cells_.size() == 4 && npm.cells_.size() == npm.num_cells_);
  ASSERT_EQ(npm.cells_[0].key, 5);
  ASSERT_EQ(npm.cells_[1].key, 9);
  ASSERT_EQ(npm.cells_[2].key, 13);
  ASSERT_EQ(npm.cells_[3].key, 19);
  ASSERT_NE(npm.cells_[0].left_child, NULL_PAGE);
  ASSERT_NE(npm.cells_[1].left_child, NULL_PAGE);
  ASSERT_NE(npm.cells_[2].left_child, NULL_PAGE);
  ASSERT_NE(npm.cells_[3].left_child, NULL_PAGE);
  ASSERT_NE(npm.right_child_, NULL_PAGE);

  assert_leaf_page(
    npm.cells_[0].left_child, pager.get(), db_file_ptr,
    {1, 3},
    {sizeof("111") - 1, sizeof("333") - 1},
    {"111", "333"}
  );

  assert_leaf_page(
    npm.cells_[1].left_child, pager.get(), db_file_ptr,
    {5, 7},
    {sizeof("555") - 1, sizeof("777") - 1},
    {"555", "777"}
  );

  assert_leaf_page(
    npm.cells_[2].left_child, pager.get(), db_file_ptr,
    {9, 11},
    {sizeof("999") - 1, sizeof("111111") - 1},
    {"999", "111111"}
  );

  assert_leaf_page(
    npm.cells_[3].left_child, pager.get(), db_file_ptr,
    {13, 14, 15},
    {sizeof("131313") - 1, sizeof("141414") - 1, sizeof("151515") - 1},
    {"131313", "141414", "151515"}
  );

  assert_leaf_page(
    npm.right_child_, pager.get(), db_file_ptr,
    {20, 21, 23},
    {sizeof("202020") - 1, sizeof("212121") - 1, sizeof("232323") - 1},
    {"202020", "212121", "232323"}
  );
}

TEST_F(BTreeBuilderTest, BTreeBuildBasic3) {
  ASSERT_NE(db_file_ptr, nullptr);
  ASSERT_NE(pager, nullptr);

  BTreeConfig config;
  config.leaf_min_cells = 0;
  config.node_min_cells = 0;

  PageNumber root_pgno = BTreeBuilder::build_tree(
    "tests/btree/configs/basic3.yaml",
    pager.get(),
    config
  );

  ASSERT_EQ(pager->get_page_type(root_pgno), PAGER_NODE_PAGE);

  NodePageManager npm(root_pgno, db_file_ptr);
  ASSERT_TRUE(npm.cells_.size() == 2 && npm.cells_.size() == npm.num_cells_);
  ASSERT_EQ(npm.cells_[0].key, 4);
  ASSERT_EQ(npm.cells_[1].key, 9);
  ASSERT_NE(npm.cells_[0].left_child, NULL_PAGE);
  ASSERT_NE(npm.cells_[1].left_child, NULL_PAGE);
  ASSERT_NE(npm.right_child_, NULL_PAGE);

  assert_leaf_page(
    npm.cells_[0].left_child, pager.get(), db_file_ptr,
    {1, 3},
    {sizeof("111") - 1, sizeof("333") - 1},
    {"111", "333"}
  );

  assert_leaf_page(
    npm.cells_[1].left_child, pager.get(), db_file_ptr,
    {5, 7},
    {sizeof("555") - 1, sizeof("777") - 1},
    {"555", "777"}
  );

  assert_leaf_page(
    npm.right_child_, pager.get(), db_file_ptr,
    {9, 12, 14},
    {sizeof("999") - 1, sizeof("121212") - 1, sizeof("141414") - 1},
    {"999", "121212", "141414"}
  );
}
