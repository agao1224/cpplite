#include <cassert>

#include "btree.h"
#include "pager/base_page.h"
#include "pager/node_page/node_page.h"

BTreeCursor::BTreeCursor(Pager* pager, PageNumber root_pgno) {
  assert(pager != nullptr);

  pager->seek_page(root_pgno);
  NodePageManager npm = std::get<NodePageManager>(pager->page_manager_);
  assert(npm.page_type_ == PAGER_NODE_PAGE);

  pager_ = pager;
  root_pgno_ = root_pgno;
}

BTreeCursor::~BTreeCursor() = default;

void BTreeCursor::move_to_first() {
  assert(pager_ != nullptr);

  pager_->seek_page(root_pgno_);

  PageNumber curr_pgno = root_pgno_;
  PagerPageType curr_page_type = pager_->get_page_type(curr_pgno);
  assert(curr_page_type == PAGER_NODE_PAGE);

  BTreeCursorStack new_cursor;

  while (curr_page_type != PAGER_LEAF_PAGE) {
    if (curr_pgno != root_pgno_) {
      auto node_cell_idx_pair = std::make_pair(curr_pgno, 0);
      new_cursor.push(node_cell_idx_pair);
    }
    auto npm = std::get<NodePageManager>(pager_->page_manager_);
    curr_pgno = npm.cells_[0].left_child;
    curr_page_type = pager_->get_page_type(curr_pgno);
  }
  cursor_ = new_cursor;
  return;
}

void BTreeCursor::move_to_last() {
  assert(pager_ != nullptr);
  pager_->seek_page(root_pgno_);

  PageNumber curr_pgno = root_pgno_;
  PagerPageType curr_page_type = pager_->get_page_type(curr_pgno);
  size_t curr_page_cell_idx = 0;
  assert(curr_page_type == PAGER_NODE_PAGE);

  BTreeCursorStack new_cursor;

  while (curr_page_type != PAGER_LEAF_PAGE) {
    if (curr_pgno != root_pgno_) {
      auto node_cell_idx_pair = std::make_pair(curr_pgno, curr_page_cell_idx);
      new_cursor.push(node_cell_idx_pair);
    }
    auto npm = std::get<NodePageManager>(pager_->page_manager_);
    curr_pgno = npm.cells_[npm.cells_.size()-1].left_child;
    curr_page_type = pager_->get_page_type(curr_pgno);
  }
  cursor_ = new_cursor;
  return;
}

// TODO(andrew):
// - Implement a binary search that looks for the key,
// - If the key is not found, we want to return the
// - first node cell that is GREATER than the key
// - to follow its left_child pointer down
//
// return type should be: PageNumber (the page we should
// be exploring down into next)
//
// * In the case where we're at the end of the list
//   and there's no first greater cell, then the page
//   we need to traverse down into is just node's right_child

bool BTreeCursor::move_to_key(DefaultPagerKey key) {
  assert(pager_ != nullptr);
  // TODO(andrew): this is starting from the root, an
  // "absolute" search for the key
  pager_->seek_page(root_pgno_);
  PagerPageType curr_page_type = pager_->get_page_type(root_pgno_);
  assert(curr_page_type == PAGER_NODE_PAGE);

  auto npm = std::get<NodePageManager>(pager_->page_manager_);
  size_t lo = 0;
  size_t hi = npm.num_cells_;
  assert(npm.num_cells_ == npm.cells_.size());

  std::optional<std::pair<PageNumber, size_t>> curr_node_idx_pair;
  BTreeCursorStack candidate_cursor;

  bool key_not_found = true;

  while (key_not_found) {
    // TODO(andrew):
    // call that binary search for first greater cell here
    // returns page we should explore
    // explore that page by using that page's manager
    // - at each iteration we're adding to the cursor stack
    // - if explore page is leaf node, then we binary search
    // on the leaf cells until key cell is found. if not return false
    // *** IF NOT FOUND, WE NEED TO RESET THE CURSOR. IT SHOULD NOT HAVE
    // MODIFIED STATE IF THE SEARCH FAILS. ONLY MODIFY THE CURSOR
    // IF WE HAVE ACTUALLY FOUND THE LEAF CELL WITH MATCHING KEY ***
  }
  return false;
}