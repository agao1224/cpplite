#include <cstddef>
#include <algorithm>
#include <memory>
#include <cmath>
#include <gtest/gtest.h>

#include "db_test_fixture.h"
#include "pager/base_page.h"
#include "pager/overflow_page/overflow_page.h"
#include "pager/pager.h"
#include "pager/node_page/node_page.h"
#include "test_utils.h"

class OverflowPageManagerTest : public DbTestFixture {};

TEST_F(OverflowPageManagerTest, OverflowPageManagerBasic) {
  ASSERT_NE(db_file_ptr, nullptr);

  std::vector<std::byte> random_payload = generate_random_payload(1000);
  PageNumber op1 = pager->create_page(PAGER_OVERFLOW_PAGE, random_payload);

  OverflowPageManager opm(op1, db_file_ptr);
  ASSERT_EQ(opm.checksum_, CHECKSUM);
  ASSERT_EQ(opm.pgno_, op1);
  ASSERT_EQ(opm.next_overflow_page_, NULL_PAGE);
  ASSERT_NE(opm.db_file_ptr_, nullptr);
  ASSERT_EQ(opm.pgno_, 2);
  ASSERT_TRUE(opm.get_overflow_content() == random_payload);
}

TEST_F(OverflowPageManagerTest, OverflowPageManagerMultiplePages) {
  size_t PAYLOAD_SIZE = 10000;
  std::vector<std::byte> random_payload = generate_random_payload(PAYLOAD_SIZE);

  std::vector<std::vector<std::byte>> expected_chunks;
  size_t overflow_payload_size = PAGE_SIZE - sizeof(PagerOverflowPageHeader_t);
  for (size_t i = 0; i < random_payload.size(); i += overflow_payload_size) {
    std::vector<std::byte> curr_chunk(
      random_payload.begin() + i,
      random_payload.begin() + std::min(i + overflow_payload_size, (size_t) PAYLOAD_SIZE)
    );
    expected_chunks.push_back(curr_chunk);
  }

  PageNumber op = pager->create_page(PAGER_OVERFLOW_PAGE, random_payload);
  size_t payload_idx = 0;

  while (op != NULL_PAGE) {
    OverflowPageManager opm(op, db_file_ptr);
    ASSERT_EQ(opm.get_num_bytes(), expected_chunks[payload_idx].size());
    ASSERT_TRUE(opm.get_overflow_content() == expected_chunks[payload_idx]);
    payload_idx++;
    op = opm.get_next_overflow_page();
  }
  ASSERT_EQ(payload_idx, (size_t) std::ceil((float) PAYLOAD_SIZE / overflow_payload_size));
}
