#include <cstddef>
#include <gtest/gtest.h>

#include "db_test_fixture.h"
#include "pager/base_page.h"
#include "pager/overflow_page/overflow_page.h"
#include "pager/pager.h"
#include "pager/leaf_page/leaf_page.h"
#include "test_utils.h"

class LeafPageManagerTest : public DbTestFixture {};

TEST_F(LeafPageManagerTest, LeafPageManagerBasic) {
  ASSERT_NE(db_file_ptr, nullptr);

  PageNumber lp1 = pager->create_page(PAGER_LEAF_PAGE);

  LeafPageManager lpm(lp1, db_file_ptr, pager.get());
  ASSERT_EQ(lpm.get_free_space(), PAGE_SIZE - sizeof(PagerLeafPageHeader_t));
  ASSERT_EQ(lpm.num_cells_, 0);
  ASSERT_EQ(lpm.cells_.size(), 0);
  ASSERT_EQ(lpm.pgno_, 2);
}

TEST_F(LeafPageManagerTest, LeafPageManagerInsert1) {
  std::vector<std::byte> random_payload = generate_random_payload(1000);

  PageNumber lp1 = pager->create_page(PAGER_LEAF_PAGE);

  LeafPageManager lpm(lp1, db_file_ptr, pager.get());
  size_t prev_free_space = lpm.get_free_space();

  ASSERT_EQ(prev_free_space, PAGE_SIZE - sizeof(PagerLeafPageHeader_t));

  bool insert_ok = lpm.insert_cell(1, random_payload);
  ASSERT_TRUE(insert_ok);
  size_t new_free_space = lpm.get_free_space();
  ASSERT_EQ(lpm.num_cells_, 1);
  ASSERT_EQ(new_free_space, prev_free_space - sizeof(LeafCell_t));

  LeafCell_t cell = lpm.cells_[0];
  ASSERT_EQ(cell.key, 1);
  ASSERT_EQ(cell.payload_size, random_payload.size());
  ASSERT_EQ(cell.record_page, 3);
}

TEST_F(LeafPageManagerTest, LeafPageManagerInsert2) {
  size_t PAYLOAD_SIZE = 100000;
  std::vector<std::byte> random_payload = generate_random_payload(PAYLOAD_SIZE);

  PageNumber lp1 = pager->create_page(PAGER_LEAF_PAGE);
  LeafPageManager lpm(lp1, db_file_ptr, pager.get());

  bool insert_ok = lpm.insert_cell(1, random_payload);
  ASSERT_TRUE(insert_ok);

  std::optional<std::vector<std::byte>> opt_payload = lpm.get_payload(1);
  ASSERT_TRUE(opt_payload.has_value());
  ASSERT_TRUE(opt_payload.value() == random_payload);
}

TEST_F(LeafPageManagerTest, LeafPageManagerInsert3) {
  size_t PAYLOAD_SIZE_LIMIT = 20000; 
  PageNumber lp1 = pager->create_page(PAGER_LEAF_PAGE);
  LeafPageManager lpm(lp1, db_file_ptr, pager.get());
  size_t overflow_payload_size = PAGE_SIZE - sizeof(PagerOverflowPageHeader_t);

  for (size_t i = 0; i < 100; i++) {
    std::vector<std::byte> random_payload = generate_random_payload(
      generate_random_payload_size(PAYLOAD_SIZE_LIMIT)
    );
    bool insert_ok = lpm.insert_cell(i, random_payload);
    ASSERT_TRUE(insert_ok);

    ASSERT_EQ(lpm.num_cells_, i+1);
    LeafCell_t cell = lpm.cells_[i];

    ASSERT_EQ(cell.payload_size, random_payload.size());
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
    std::optional<std::vector<std::byte>> opt_payload = lpm.get_payload(i);
    ASSERT_TRUE(opt_payload.has_value());
    ASSERT_TRUE(opt_payload.value() == random_payload);
  }
}

TEST_F(LeafPageManagerTest, LeafPageManagerGetPrevNext) {
  PageNumber prev_pgno = NULL_PAGE;
  PageNumber first_leaf_page = NULL_PAGE;
  PageNumber last_leaf_page = NULL_PAGE;

  size_t pages_to_create = 10;
  for (size_t i = 0; i < pages_to_create; i++) {
    PageNumber leaf_pgno = pager->create_page(PAGER_LEAF_PAGE);
    if (i == 0)
      first_leaf_page = leaf_pgno;
    else if (i == pages_to_create-1)
      last_leaf_page = leaf_pgno;

    LeafPageManager lpm_curr(leaf_pgno, db_file_ptr, pager.get());
    lpm_curr.set_prev_page(prev_pgno);

    if (prev_pgno != NULL_PAGE) {
      LeafPageManager lpm_prev(prev_pgno, db_file_ptr, pager.get());
      lpm_prev.set_next_page(leaf_pgno);
    }
    prev_pgno = leaf_pgno;
  }

  size_t expected_number_of_pages = 0;
  PageNumber curr_pgno = first_leaf_page;
  while (curr_pgno != NULL_PAGE) {
    LeafPageManager lpm_curr(curr_pgno, db_file_ptr, pager.get());
    curr_pgno = lpm_curr.next_page();
    expected_number_of_pages++;
  }
  ASSERT_EQ(expected_number_of_pages, pages_to_create);

  expected_number_of_pages = 0;
  curr_pgno = last_leaf_page;
  while (curr_pgno != NULL_PAGE) {
    LeafPageManager lpm_curr(curr_pgno, db_file_ptr, pager.get());
    curr_pgno = lpm_curr.prev_page();
    expected_number_of_pages++;
  }
  ASSERT_EQ(expected_number_of_pages, pages_to_create);
}

TEST_F(LeafPageManagerTest, WriteCell_PreservesRecordPage) {
  std::vector<std::byte> payload = generate_random_payload(1000);

  PageNumber src_leaf = pager->create_page(PAGER_LEAF_PAGE);
  LeafPageManager src_lpm(src_leaf, db_file_ptr, pager.get());
  src_lpm.insert_cell(42, payload);
  LeafCell_t original_cell = src_lpm.cells_[0];

  PageNumber dst_leaf = pager->create_page(PAGER_LEAF_PAGE);
  LeafPageManager dst_lpm(dst_leaf, db_file_ptr, pager.get());
  bool ok = dst_lpm.write_cell(original_cell);
  ASSERT_TRUE(ok);

  ASSERT_EQ(dst_lpm.num_cells_, 1);
  ASSERT_EQ(dst_lpm.cells_[0].key, 42);
  ASSERT_EQ(dst_lpm.cells_[0].payload_size, payload.size());
  ASSERT_EQ(dst_lpm.cells_[0].record_page, original_cell.record_page);
}

TEST_F(LeafPageManagerTest, WriteCell_PayloadStillReadable) {
  std::vector<std::byte> payload = generate_random_payload(50000);

  PageNumber src_leaf = pager->create_page(PAGER_LEAF_PAGE);
  LeafPageManager src_lpm(src_leaf, db_file_ptr, pager.get());
  src_lpm.insert_cell(7, payload);
  LeafCell_t original_cell = src_lpm.cells_[0];

  PageNumber dst_leaf = pager->create_page(PAGER_LEAF_PAGE);
  LeafPageManager dst_lpm(dst_leaf, db_file_ptr, pager.get());
  dst_lpm.write_cell(original_cell);

  std::optional<std::vector<std::byte>> result = dst_lpm.get_payload(7);
  ASSERT_TRUE(result.has_value());
  ASSERT_EQ(result.value(), payload);
}

TEST_F(LeafPageManagerTest, WriteCell_Multiple) {
  size_t num_cells = 5;
  std::vector<LeafCell_t> cells;

  PageNumber src_leaf = pager->create_page(PAGER_LEAF_PAGE);
  LeafPageManager src_lpm(src_leaf, db_file_ptr, pager.get());
  for (size_t i = 0; i < num_cells; i++) {
    src_lpm.insert_cell(i, generate_random_payload(500));
    cells.push_back(src_lpm.cells_[i]);
  }

  PageNumber dst_leaf = pager->create_page(PAGER_LEAF_PAGE);
  LeafPageManager dst_lpm(dst_leaf, db_file_ptr, pager.get());
  for (auto& c : cells)
    ASSERT_TRUE(dst_lpm.write_cell(c));

  LeafPageManager dst_lpm_reloaded(dst_leaf, db_file_ptr, pager.get());
  ASSERT_EQ(dst_lpm_reloaded.num_cells_, num_cells);
  for (size_t i = 0; i < num_cells; i++) {
    ASSERT_EQ(dst_lpm_reloaded.cells_[i].key, cells[i].key);
    ASSERT_EQ(dst_lpm_reloaded.cells_[i].record_page, cells[i].record_page);
    ASSERT_EQ(dst_lpm_reloaded.cells_[i].payload_size, cells[i].payload_size);
  }
}

TEST_F(LeafPageManagerTest, WriteCell_FreeSpaceDecreases) {
  PageNumber leaf = pager->create_page(PAGER_LEAF_PAGE);
  LeafPageManager src_lpm(leaf, db_file_ptr, pager.get());
  src_lpm.insert_cell(1, generate_random_payload(100));
  LeafCell_t cell = src_lpm.cells_[0];

  PageNumber dst_leaf = pager->create_page(PAGER_LEAF_PAGE);
  LeafPageManager dst_lpm(dst_leaf, db_file_ptr, pager.get());
  size_t free_before = dst_lpm.get_free_space();
  dst_lpm.write_cell(cell);
  ASSERT_EQ(dst_lpm.get_free_space(), free_before - sizeof(LeafCell_t));
}

TEST_F(LeafPageManagerTest, LeafPageManagerSetPrevNext) {
  PageNumber leaf_pgno = pager->create_page(PAGER_LEAF_PAGE);
  LeafPageManager lpm(leaf_pgno, db_file_ptr, pager.get());
  ASSERT_EQ(lpm.prev_page(), NULL_PAGE);
  ASSERT_EQ(lpm.next_page(), NULL_PAGE);

  PageNumber new_leaf_pgno = pager->create_page(PAGER_LEAF_PAGE);
  LeafPageManager lpm_new(new_leaf_pgno, db_file_ptr, pager.get());
  ASSERT_EQ(lpm_new.prev_page(), NULL_PAGE);
  ASSERT_EQ(lpm_new.next_page(), NULL_PAGE);

  lpm.set_next_page(new_leaf_pgno);
  lpm_new.set_prev_page(leaf_pgno);
  ASSERT_EQ(lpm.next_page(), new_leaf_pgno);
  ASSERT_EQ(lpm_new.prev_page(), leaf_pgno);
}
