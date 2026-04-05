#include <cassert>
#include <stdexcept>
#include <iostream>
#include <utility>

#include "btree.h"
#include "pager/base_page.h"
#include "pager/leaf_page/leaf_page.h"
#include "pager/node_page/node_page.h"

BTreeCursor::BTreeCursor(Pager* pager, PageNumber root_pgno, BTreeConfig config) : config_(config) {
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

  BTreeCursorStack new_cursor;

  assert(curr_page_type == PAGER_NODE_PAGE);
  while (curr_page_type != PAGER_LEAF_PAGE) {
    pager_->seek_page(curr_pgno);
    auto npm = std::get<NodePageManager>(pager_->page_manager_);
    assert(npm.num_cells_ == npm.cells_.size() && npm.num_cells_ > 0);

    auto node_cell_idx_pair = std::make_pair(curr_pgno, 0);
    new_cursor.push(node_cell_idx_pair);

    curr_pgno = npm.cells_[0].left_child;
    curr_page_type = pager_->get_page_type(curr_pgno);
  }

  assert(curr_page_type == PAGER_LEAF_PAGE);
  pager_->seek_page(curr_pgno);
  auto lpm = std::get<LeafPageManager>(pager_->page_manager_);

  assert(lpm.num_cells_ == lpm.cells_.size() && lpm.num_cells_ > 0);
  new_cursor.push(std::make_pair(curr_pgno, 0));
  cursor_ = new_cursor;
  return;
}

void BTreeCursor::move_to_last() {
  assert(pager_ != nullptr);

  pager_->seek_page(root_pgno_);

  PageNumber curr_pgno = root_pgno_;
  PagerPageType curr_page_type = pager_->get_page_type(curr_pgno);
  size_t curr_page_num_cells;

  BTreeCursorStack new_cursor;

  assert(curr_page_type == PAGER_NODE_PAGE);
  while (curr_page_type != PAGER_LEAF_PAGE) {
    pager_->seek_page(curr_pgno);
    auto npm = std::get<NodePageManager>(pager_->page_manager_);
    assert(npm.num_cells_ == npm.cells_.size() && npm.num_cells_ > 0);

    // NOTE(andrew): When moving to the rightmost leaf, we're
    // using the current page's number of cells as the "cell index".
    // This is simply a sentinel value to indicate that we came from the
    // rightmost child of our parent.
    auto node_cell_idx_pair = std::make_pair(curr_pgno, npm.num_cells_);
    new_cursor.push(node_cell_idx_pair);

    curr_pgno = npm.right_child_;
    curr_page_type = pager_->get_page_type(curr_pgno);
  }

  assert(curr_page_type == PAGER_LEAF_PAGE);
  pager_->seek_page(curr_pgno);
  auto lpm = std::get<LeafPageManager>(pager_->page_manager_);

  assert(lpm.num_cells_ == lpm.cells_.size() && lpm.num_cells_ > 0);
  new_cursor.push(std::make_pair(curr_pgno, lpm.num_cells_ - 1));
  cursor_ = new_cursor;
  return;
}

size_t find_child_to_traverse(DefaultPagerKey target_key, std::vector<NodeCell_t> cells, PageNumber right_child) {
  size_t lo = 0;
  size_t hi = cells.size();
  while (lo < hi) {
    size_t mid = lo + (hi - lo)/2;
    if (target_key < cells[mid].key) hi = mid;
    else lo = mid+1;
  }
  return lo;
}

std::optional<size_t> find_matching_leaf_cell(DefaultPagerKey target_key, std::vector<LeafCell_t> cells) {
  size_t lo = 0;
  size_t hi = cells.size();
  while (lo < hi) {
    size_t mid = lo + (hi - lo)/2;
    if (target_key < cells[mid].key) hi = mid;
    else if (cells[mid].key < target_key) lo = mid+1;
    else return mid;
  }
  return std::nullopt;
}

bool BTreeCursor::move_to_key(DefaultPagerKey key) {
  assert(pager_ != nullptr);
  // TODO(andrew): this is starting from the root, an
  // "absolute" search for the key
  pager_->seek_page(root_pgno_);
  PagerPageType curr_page_type = pager_->get_page_type(root_pgno_);
  assert(curr_page_type == PAGER_NODE_PAGE);

  BTreeCursorStack candidate_cursor;
  PageNumber prev_page = root_pgno_;

  bool key_not_found = true;
  while (key_not_found) {
    if (curr_page_type == PAGER_NODE_PAGE) {
      auto npm = std::get<NodePageManager>(pager_->page_manager_);
      size_t idx_to_traverse = find_child_to_traverse(
        key, npm.cells_, npm.right_child_
      );
      candidate_cursor.push(
        std::make_pair(prev_page, idx_to_traverse)
      );

      PageNumber child_page;
      assert(npm.num_cells_ == npm.cells_.size());
      if (idx_to_traverse == npm.num_cells_)
        child_page = npm.right_child_;
      else
        child_page = npm.cells_[idx_to_traverse].left_child;

      pager_->seek_page(child_page);
      prev_page = child_page;
      curr_page_type = pager_->get_page_type(child_page);
    } else {
      auto lpm = std::get<LeafPageManager>(pager_->page_manager_);
      std::optional<size_t> cell_idx_opt = find_matching_leaf_cell(key, lpm.cells_);
      if (!cell_idx_opt.has_value()) return false;

      size_t cell_idx = cell_idx_opt.value();
      candidate_cursor.push(
        std::make_pair(prev_page, cell_idx)
      );
      key_not_found = false;
    }
  }
  cursor_ = candidate_cursor;
  return true;
}

bool BTreeCursor::prev() {
  assert(pager_ != nullptr);
  assert(cursor_.size() > 0);

  BTreeCursorStackElt curr = cursor_.top();
  PagerPageType page_type = pager_->get_page_type(curr.first);
  assert(page_type == PAGER_LEAF_PAGE);

  LeafPageManager curr_lpm(curr.first, pager_->db_file_ptr_, pager_);
  assert(curr_lpm.cells_.size() == curr_lpm.num_cells_);
  assert(0 <= curr.second && curr.second < curr_lpm.cells_.size());

  if (curr.second > 0) {
    cursor_.pop();
    BTreeCursorStackElt new_curr = std::make_pair(
      curr.first,
      curr.second-1
    );
    cursor_.push(new_curr);
    return true;
  }

  assert(curr.second == 0);
  // NOTE(andrew): pop leaf and walk up until we find an ancestor with a nonzero
  // cell index (meaning there's a left subtree to descend into)
  cursor_.pop();
  size_t curr_cell_idx = 0;
  while (cursor_.size() > 0) {
    curr = cursor_.top();
    curr_cell_idx = curr.second;
    if (curr_cell_idx > 0) break;
    cursor_.pop();
  }

  // NOTE(andrew): already at leftmost cell in tree
  if (cursor_.size() == 0) return false;

  assert(curr.second > 0);
  cursor_.pop();
  BTreeCursorStackElt new_ancestor = std::make_pair(
    curr.first,
    curr.second-1
  );
  cursor_.push(new_ancestor);

  assert(pager_->get_page_type(new_ancestor.first) == PAGER_NODE_PAGE);
  NodePageManager npm(new_ancestor.first, pager_->db_file_ptr_);
  PageNumber child_page = npm.cells_[new_ancestor.second].left_child;

  BTreeCursorStackElt new_child;
  while (true) {
    if (pager_->get_page_type(child_page) == PAGER_LEAF_PAGE) {
      LeafPageManager lpm(child_page, pager_->db_file_ptr_, pager_);
      new_child = std::make_pair(child_page, lpm.num_cells_-1);
      cursor_.push(new_child);
      break;
    }

    npm = NodePageManager(child_page, pager_->db_file_ptr_); 
    new_child = std::make_pair(child_page, npm.num_cells_);
    cursor_.push(new_child);
    child_page = npm.right_child_;
  }
  return true;
}

DefaultPagerKey BTreeCursor::current_key() const {
  assert(cursor_.size() > 0);

  BTreeCursorStackElt curr = cursor_.top();
  PagerPageType page_type = pager_->get_page_type(curr.first);
  assert (page_type == PAGER_LEAF_PAGE);

  LeafPageManager lpm(curr.first, pager_->db_file_ptr_, pager_);
  if (0 <= curr.second && curr.second < lpm.cells_.size())
    return lpm.cells_[curr.second].key;

  throw new std::runtime_error("[btree:current_key]: Cell index out of range");
}

PageNumber BTreeCursor::current_pgno() const {
  assert(cursor_.size() > 0);

  BTreeCursorStackElt curr = cursor_.top();
  PagerPageType page_type = pager_->get_page_type(curr.first);
  assert (page_type == PAGER_LEAF_PAGE);

  return curr.first;
}

PageNumber BTreeCursor::current_record_pgno() const {
  assert(cursor_.size() > 0);

  BTreeCursorStackElt curr = cursor_.top();
  PagerPageType page_type = pager_->get_page_type(curr.first);
  assert(page_type == PAGER_LEAF_PAGE);

  LeafPageManager lpm(curr.first, pager_->db_file_ptr_, pager_);
  if (0 <= curr.second && curr.second < lpm.cells_.size())
    return lpm.cells_[curr.second].record_page;

  throw new std::runtime_error("[btree:current_record_pgno]: Cell index out of range");
}