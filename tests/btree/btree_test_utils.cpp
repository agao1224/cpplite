#include <gtest/gtest.h>

#include "pager/overflow_page/overflow_page.h"
#include "btree_test_utils.h"

std::vector<std::byte> str_to_bytes(const std::string& s) {
  return {
    reinterpret_cast<const std::byte*>(s.data()),
    reinterpret_cast<const std::byte*>(s.data() + s.size())
  };
}

void assert_payload(PageNumber record_page, std::shared_ptr<OsFile> db_file_ptr, const std::string& expected) {
  OverflowPageManager opm(record_page, db_file_ptr);
  EXPECT_EQ(opm.get_overflow_content(), str_to_bytes(expected));
}

void assert_cursor_stack(BTreeCursorStack stack, std::vector<BTreeCursorStackElt> expected_top_to_bottom) {
  ASSERT_EQ(stack.size(), expected_top_to_bottom.size());
  for (size_t i = 0; i < expected_top_to_bottom.size(); i++) {
    EXPECT_EQ(stack.top().first, expected_top_to_bottom[i].first) << "pgno mismatch at stack depth " << i;
    EXPECT_EQ(stack.top().second, expected_top_to_bottom[i].second) << "cell_idx mismatch at stack depth " << i;
    stack.pop();
  }
}
