#include <cstddef>
#include <memory>
#include <random>
#include <chrono>
#include <gtest/gtest.h>

#include "db_test_fixture.h"
#include "pager/base_page.h"
#include "pager/overflow_page/overflow_page.h"
#include "pager/pager.h"
#include "pager/node_page/node_page.h"
#include "test_utils.h"

class NodePageManagerTest : public DbTestFixture {};

size_t generate_random_payload_size(size_t payload_size_limit) {
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::mt19937 gen(seed);
  std::uniform_int_distribution<> distrib(1, payload_size_limit);
  return (size_t) distrib(gen);
}


TEST_F(NodePageManagerTest, NodePageManagerBasic) {
  ASSERT_NE(db_file_ptr, nullptr);

  PageNumber np1 = pager->create_page(PAGER_NODE_PAGE);

  NodePageManager npm(np1, db_file_ptr, pager.get());
  ASSERT_EQ(npm.get_free_space(), PAGE_SIZE - sizeof(PagerNodePageHeader_t));
  ASSERT_EQ(npm.num_cells_, 0);
  ASSERT_EQ(npm.cells_.size(), 0);
  ASSERT_EQ(npm.pgno_, 2);
}

TEST_F(NodePageManagerTest, NodePageManagerInsert1) {
  std::vector<std::byte> random_payload = generate_random_payload(1000);

  PageNumber np1 = pager->create_page(PAGER_NODE_PAGE);

  NodePageManager npm(np1, db_file_ptr, pager.get());
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

TEST_F(NodePageManagerTest, NodePageManagerInsert2) {
  size_t PAYLOAD_SIZE = 100000;
  std::vector<std::byte> random_payload = generate_random_payload(PAYLOAD_SIZE);

  PageNumber np1 = pager->create_page(PAGER_NODE_PAGE);
  NodePageManager npm(np1, db_file_ptr, pager.get());

  bool insert_ok = npm.insert_cell(1, random_payload);
  ASSERT_TRUE(insert_ok);

  std::optional<std::vector<std::byte>> opt_payload = npm.get_payload(1);
  ASSERT_TRUE(opt_payload.has_value());
  ASSERT_TRUE(opt_payload.value() == random_payload);
}

TEST_F(NodePageManagerTest, NodePageManagerInsert3) {
  size_t PAYLOAD_SIZE_LIMIT = 20000; 
  PageNumber np1 = pager->create_page(PAGER_NODE_PAGE);
  NodePageManager npm(np1, db_file_ptr, pager.get());
  size_t overflow_payload_size = PAGE_SIZE - sizeof(PagerOverflowPageHeader_t);

  for (size_t i = 0; i < 100; i++) {
    std::vector<std::byte> random_payload = generate_random_payload(
      generate_random_payload_size(PAYLOAD_SIZE_LIMIT)
    );
    bool insert_ok = npm.insert_cell(i, random_payload);
    ASSERT_TRUE(insert_ok);

    ASSERT_EQ(npm.num_cells_, i+1);
    PagerCell_t cell = npm.cells_[i];

    ASSERT_EQ(cell.size, random_payload.size());
    ASSERT_NE(cell.record_page, NULL_PAGE);

    PageNumber op = cell.record_page;
    size_t num_pages = 0;
    while (op != NULL_PAGE) {
      OverflowPageManager opm(op, db_file_ptr);
      num_pages++;
      op = opm.get_next_overflow_page();
    }
    ASSERT_EQ(num_pages, (size_t) std::ceil(
      (float) random_payload.size() / overflow_payload_size)
    );
    std::optional<std::vector<std::byte>> opt_payload = npm.get_payload(i);
    ASSERT_TRUE(opt_payload.has_value());
    ASSERT_TRUE(opt_payload.value() == random_payload);
  }
}