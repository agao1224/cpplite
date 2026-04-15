#include <cassert>
#include <algorithm>
#include <stack>
#include <map>
#include <variant>

#include "btree.h"
#include "pager/base_page.h"
#include "pager/leaf_page/leaf_page.h"
#include "pager/node_page/node_page.h"
#include "shared.h"

SplitCellRes BTreeCursor::split_cell(
  PageNumber pgno,
  std::variant<NodeCell_t, LeafCell_t> cell
) {
  assert(pager_ != nullptr);
  std::map<
    DefaultPagerKey,
    std::variant<NodeCell_t, LeafCell_t>
  > cell_map;
  std::vector<DefaultPagerKey> keys;

  if (std::holds_alternative<LeafCell_t>(cell)) {
    LeafPageManager lpm(pgno, pager_->db_file_ptr_, pager_);
    for (LeafCell_t leaf_cell : lpm.cells_) {
      keys.push_back(leaf_cell.key);
      cell_map[leaf_cell.key] = leaf_cell;
    }

    LeafCell_t new_leaf_cell = std::get<LeafCell_t>(cell);
    keys.push_back(new_leaf_cell.key);
    cell_map[new_leaf_cell.key] = new_leaf_cell;
  } else {
    assert(std::holds_alternative<NodeCell_t>(cell));
    NodePageManager npm(pgno, pager_->db_file_ptr_);
    for (NodeCell_t node_cell : npm.cells_) {
      keys.push_back(node_cell.key);
      cell_map[node_cell.key] = node_cell;
    }

    NodeCell_t new_node_cell = std::get<NodeCell_t>(cell);
    keys.push_back(new_node_cell.key);
    cell_map[new_node_cell.key] = new_node_cell;
  }

  std::sort(keys.begin(), keys.end());
  // NOTE(andrew): leaf splits use ceil(n/2) because the separator key is copied
  // up to the parent but stays in the right leaf (B+tree invariant). internal
  // node splits use floor(n/2) because the separator is removed from both
  // children and only lives in the parent.
  size_t left_size = std::holds_alternative<LeafCell_t>(cell)
    ? (keys.size() + 1) / 2
    : keys.size() / 2;
  DefaultPagerKey pivot_key = keys[left_size];
  std::vector<DefaultPagerKey> left_keys(keys.begin(), keys.begin() + left_size);
  std::vector<DefaultPagerKey> right_keys(keys.begin() + left_size, keys.end());
  return SplitCellRes{
    left_keys,
    right_keys,
    cell_map
  };
}

NodeCell_t BTreeCursor::assign_children_nodes(
  PageNumber left_node,
  PageNumber right_node,
  BTreeCursorNode parent,
  DefaultPagerKey separator_key
) {
  assert(pager_ != nullptr);
  assert(pager_->get_page_type(parent.first) == PAGER_NODE_PAGE);

  NodePageManager npm(parent.first, pager_->db_file_ptr_);
  assert(npm.num_cells_ == npm.cells_.size());

  DefaultPagerKey old_parent_key;
  bool is_right_child = (parent.second == npm.num_cells_);

  if (is_right_child) {
    old_parent_key = npm.cells_[npm.num_cells_-1].key;
    assert(old_parent_key < separator_key);

    NodeCell_t right_parent;
    right_parent.key = separator_key;
    right_parent.left_child = left_node;

    npm.set_node_right_child(right_node);
    return right_parent;
  } else {
    old_parent_key = npm.cells_[parent.second].key;
    assert(separator_key < old_parent_key);

    NodeCell_t left_parent;
    left_parent.key = separator_key;
    left_parent.left_child = left_node;
    npm.set_cell_left_child(old_parent_key, right_node);
    return left_parent;
  }
}

void BTreeCursor::split_cursor_leaf(LeafCell_t new_cell) {
  assert(pager_ != nullptr);

  assert(cursor_.size() > 0);
  BTreeCursorStack cursor_clone(cursor_);
  PageNumber pgno = cursor_clone.top().first;
  assert(pager_->get_page_type(pgno) == PAGER_LEAF_PAGE);

  cursor_clone.pop();

  LeafPageManager curr_lpm(pgno, pager_->db_file_ptr_, pager_);
  assert(
    curr_lpm.cells_.size() == curr_lpm.num_cells_
    && curr_lpm.num_cells_ == config_.leaf_max_cells
  );

  auto [
    left_keys,
    right_keys,
    leaf_cell_map
  ] = split_cell(pgno, new_cell);

  assert(right_keys.size() > 0);
  DefaultPagerKey separator_key = right_keys[0];

  PageNumber left_leaf = pager_->create_page(PAGER_LEAF_PAGE);
  PageNumber right_leaf = pager_->create_page(PAGER_LEAF_PAGE);
  PageNumber old_prev_page = curr_lpm.prev_page();
  PageNumber old_next_page = curr_lpm.next_page();

  LeafPageManager left_lpm(left_leaf, pager_->db_file_ptr_, pager_);
  LeafPageManager right_lpm(right_leaf, pager_->db_file_ptr_, pager_);

  for (auto key : left_keys) {
    left_lpm.write_cell(
      std::get<LeafCell_t>(leaf_cell_map[key])
    );
  }
  for (auto key : right_keys) {
    right_lpm.write_cell(
      std::get<LeafCell_t>(leaf_cell_map[key])
    );
  }

  if (old_prev_page != NULL_PAGE) {
    LeafPageManager old_prev_lpm(old_prev_page, pager_->db_file_ptr_, pager_);
    old_prev_lpm.set_next_page(left_leaf);
  }
  left_lpm.set_next_page(right_leaf);
  right_lpm.set_next_page(old_next_page);

  if (old_next_page != NULL_PAGE) {
    LeafPageManager old_next_lpm(old_next_page, pager_->db_file_ptr_, pager_);
    old_next_lpm.set_prev_page(right_leaf);
  }
  right_lpm.set_prev_page(left_leaf);
  left_lpm.set_prev_page(old_prev_page);

  if (cursor_clone.size() == 0) {
    PageNumber new_root_pgno = pager_->create_page(PAGER_NODE_PAGE);
    NodePageManager root_npm(new_root_pgno, pager_->db_file_ptr_);
    root_npm.insert_node_cell(separator_key);
    root_npm.set_cell_left_child(separator_key, left_leaf);
    root_npm.set_node_right_child(right_leaf);
    set_root_pgno(new_root_pgno);
    return;
  }

  BTreeCursorNode parent = cursor_clone.top();

  assert(pager_->get_page_type(parent.first) == PAGER_NODE_PAGE);
  NodeCell_t new_parent_node = assign_children_nodes(
    left_leaf, right_leaf, parent, separator_key
  );
  NodePageManager npm(parent.first, pager_->db_file_ptr_);

  if (npm.num_cells_ == config_.node_max_cells) {
    cursor_ = cursor_clone;
    split_cursor_node(new_parent_node);
  } else {
    npm.insert_node_cell(new_parent_node.key);
    npm.set_cell_left_child(new_parent_node.key, new_parent_node.left_child);
  }
  return;
}

void BTreeCursor::split_cursor_node(NodeCell_t new_cell) {
  assert(pager_ != nullptr);
  assert(cursor_.size() > 0);

  BTreeCursorStack cursor_clone(cursor_);
  PageNumber pgno = cursor_clone.top().first;
  assert(pager_->get_page_type(pgno) == PAGER_NODE_PAGE);

  cursor_clone.pop();

  NodePageManager curr_npm(pgno, pager_->db_file_ptr_);
  assert(
    curr_npm.cells_.size() == curr_npm.num_cells_
    && curr_npm.num_cells_ == config_.node_max_cells
  );

  auto [
    left_keys,
    right_keys,
    node_cell_map
  ] = split_cell(pgno, new_cell);

  assert(right_keys.size() > 0);
  // NOTE(andrew): for internal nodes, we push up
  // the separator key into parent. internal nodes also
  // do not share any identical keys with each other
  // at any depth of the tree.
  DefaultPagerKey separator_key = right_keys[0];
  right_keys.erase(right_keys.begin());

  PageNumber left_node = pager_->create_page(PAGER_NODE_PAGE);
  PageNumber right_node = pager_->create_page(PAGER_NODE_PAGE);
  NodePageManager left_npm(left_node, pager_->db_file_ptr_);
  NodePageManager right_npm(right_node, pager_->db_file_ptr_);

  for (size_t i = 0; i < left_keys.size(); i++) {
    DefaultPagerKey curr_key = left_keys[i];
    left_npm.insert_node_cell(curr_key);
    left_npm.set_cell_left_child(
      curr_key,
      (std::get<NodeCell_t>(node_cell_map[curr_key])).left_child
    );
  }
  left_npm.set_node_right_child(
    (std::get<NodeCell_t>(node_cell_map[separator_key]).left_child)
  );

  for (size_t i = 0; i < right_keys.size(); i++) {
    DefaultPagerKey curr_key = right_keys[i];
    right_npm.insert_node_cell(curr_key);
    right_npm.set_cell_left_child(
      curr_key,
      (std::get<NodeCell_t>(node_cell_map[curr_key]).left_child)
    );
  }
  right_npm.set_node_right_child(curr_npm.right_child_);

  if (cursor_clone.size() == 0) {
    PageNumber new_root_pgno = pager_->create_page(PAGER_NODE_PAGE);
    NodePageManager root_npm(new_root_pgno, pager_->db_file_ptr_);
    root_npm.insert_node_cell(separator_key);
    root_npm.set_cell_left_child(separator_key, left_node);
    root_npm.set_node_right_child(right_node);
    set_root_pgno(new_root_pgno);
    return;
  }

  assert(cursor_clone.size() > 0);
  BTreeCursorNode parent = cursor_clone.top();

  assert(pager_->get_page_type(parent.first) == PAGER_NODE_PAGE);
  NodeCell_t new_parent_node = assign_children_nodes(
    left_node, right_node, parent, separator_key
  );

  NodePageManager npm(parent.first, pager_->db_file_ptr_);
  if (npm.num_cells_ == config_.node_max_cells) {
    cursor_ = cursor_clone;
    split_cursor_node(new_parent_node);
  } else {
    npm.insert_node_cell(new_parent_node.key);
    npm.set_cell_left_child(new_parent_node.key, new_parent_node.left_child);
  }
  return;
}

bool BTreeCursor::borrow_leaf_sibling_cell(
  PageNumber curr_pgno,
  PageNumber sibling_pgno,
  BTreeCursorNode sibling_parent,
  bool is_left_sibling
) {
  if (sibling_pgno == NULL_PAGE) return false;

  LeafPageManager sibling_lpm(sibling_pgno, pager_->db_file_ptr_, pager_);
  assert(sibling_lpm.num_cells_ == sibling_lpm.cells_.size());

  if (sibling_lpm.num_cells_ <= config_.leaf_min_cells) return false;

  LeafPageManager lpm(curr_pgno, pager_->db_file_ptr_, pager_);
  assert(pager_->get_page_type(sibling_parent.first) == PAGER_NODE_PAGE);

  NodePageManager sibling_parent_npm(sibling_parent.first, pager_->db_file_ptr_);
  assert(sibling_parent_npm.num_cells_ == sibling_parent_npm.cells_.size());
  assert(sibling_parent.second < sibling_parent_npm.num_cells_);

  DefaultPagerKey old_sibling_parent_key = sibling_parent_npm.cells_[sibling_parent.second].key;
  assert(lpm.prev_page() == sibling_pgno || lpm.next_page() == sibling_pgno);

  if (is_left_sibling) {
    LeafCell_t borrowed_cell = sibling_lpm.cells_[sibling_lpm.num_cells_-1];

    sibling_lpm.delete_cell(borrowed_cell.key);
    assert(sibling_lpm.num_cells_ >= config_.leaf_min_cells);

    lpm.write_cell(borrowed_cell);
    sibling_parent_npm.update_cell_key(old_sibling_parent_key, borrowed_cell.key);
  } else {
    LeafCell_t borrowed_cell = sibling_lpm.cells_[0];

    sibling_lpm.delete_cell(borrowed_cell.key);
    assert(sibling_lpm.num_cells_ >= config_.leaf_min_cells);

    lpm.write_cell(borrowed_cell);
    DefaultPagerKey new_sibling_parent_key = sibling_lpm.cells_[0].key;
    assert(borrowed_cell.key < new_sibling_parent_key);

    sibling_parent_npm.update_cell_key(old_sibling_parent_key, new_sibling_parent_key);
  }
  return true;
}

bool BTreeCursor::borrow_from_leaf_sibling(
  BTreeCursorNode parent,
  PageNumber curr_pgno
) {
  PageNumber prev_sibling = NULL_PAGE;
  PageNumber next_sibling = NULL_PAGE;
  BTreeCursorNode prev_sibling_parent;
  BTreeCursorNode next_sibling_parent;
  assert(pager_->get_page_type(parent.first) == PAGER_NODE_PAGE);

  NodePageManager parent_npm(parent.first, pager_->db_file_ptr_);
  assert(parent_npm.num_cells_ == parent_npm.cells_.size());
  assert(parent.second < parent_npm.num_cells_);

  if (parent.second > 0) {
    prev_sibling = parent_npm.cells_[parent.second-1].left_child;
    prev_sibling_parent = std::make_pair(parent.first, parent.second-1);
  }

  if (parent.second+1 < parent_npm.num_cells_) {
    next_sibling = parent_npm.cells_[parent.second+1].left_child;
    next_sibling_parent = std::make_pair(parent.first, parent.second+1);
  } else if (parent.second+1 == parent_npm.num_cells_) {
    next_sibling = parent_npm.right_child_;
    next_sibling_parent = std::make_pair(parent.first, parent.second+1);
  }

  if (borrow_leaf_sibling_cell(curr_pgno, prev_sibling, prev_sibling_parent, true))
    return true;
  return borrow_leaf_sibling_cell(curr_pgno, next_sibling, next_sibling_parent, false);
}

bool BTreeCursor::borrow_node_sibling_cell(
  PageNumber curr_pgno,
  PageNumber sibling_pgno,
  BTreeCursorNode sibling_parent,
  bool is_left_sibling
) {
  if (sibling_pgno == NULL_PAGE) return false;

  NodePageManager curr_npm(curr_pgno, pager_->db_file_ptr_);
  NodePageManager sibling_npm(sibling_pgno, pager_->db_file_ptr_);

  assert(sibling_npm.num_cells_ >= config_.node_min_cells);
  if (sibling_npm.num_cells_ == config_.node_min_cells) return false;

  assert(pager_->get_page_type(sibling_parent.first) == PAGER_NODE_PAGE);
  NodePageManager parent_npm(sibling_parent.first, pager_->db_file_ptr_);
  assert(parent_npm.num_cells_ == parent_npm.cells_.size());
  assert(sibling_parent.second <= parent_npm.num_cells_);
  if (is_left_sibling) {
    DefaultPagerKey sibling_parent_key = parent_npm.cells_[sibling_parent.second].key;
    PageNumber sibling_right_child = sibling_npm.right_child_;

    NodeCell_t last_sibling_cell = sibling_npm.cells_[sibling_npm.num_cells_-1];
    sibling_npm.set_node_right_child(sibling_npm.cells_[sibling_npm.num_cells_-1].left_child);
    sibling_npm.delete_node_cell(last_sibling_cell.key);

    parent_npm.update_cell_key(sibling_parent_key, last_sibling_cell.key);

    curr_npm.insert_node_cell(sibling_parent_key);
    curr_npm.set_cell_left_child(sibling_parent_key, sibling_right_child);
  } else {
    PageNumber sibling_left_child = sibling_npm.cells_[0].left_child;

    NodeCell_t first_sibling_cell = sibling_npm.cells_[0];
    sibling_npm.delete_node_cell(first_sibling_cell.key);

    NodeCell_t curr_parent = parent_npm.cells_[sibling_parent.second-1];
    DefaultPagerKey curr_parent_key = curr_parent.key;
    assert(curr_parent.left_child == curr_pgno);
    parent_npm.update_cell_key(curr_parent_key, first_sibling_cell.key);

    curr_npm.insert_node_cell(curr_parent_key);
    curr_npm.set_cell_left_child(curr_parent_key, curr_npm.right_child_);
    curr_npm.set_node_right_child(sibling_left_child);
  }
  return true;
}

bool BTreeCursor::borrow_from_node_sibling(BTreeCursorNode parent, PageNumber curr_pgno) {
  assert(pager_->get_page_type(parent.first) == PAGER_NODE_PAGE);
  NodePageManager parent_npm(parent.first, pager_->db_file_ptr_);
  assert(parent_npm.num_cells_ == parent_npm.cells_.size());
  assert(parent.second <= parent_npm.num_cells_);

  PageNumber left_sibling = NULL_PAGE;
  PageNumber right_sibling = NULL_PAGE;
  BTreeCursorNode left_sibling_parent, right_sibling_parent;

  if (parent.second > 0) {
    left_sibling = parent_npm.cells_[parent.second-1].left_child;
    left_sibling_parent = std::make_pair(parent.first, parent.second-1);
    assert(pager_->get_page_type(left_sibling) == PAGER_NODE_PAGE);
  }

  if (parent.second+1 < parent_npm.num_cells_) {
    right_sibling = parent_npm.cells_[parent.second+1].left_child;
    right_sibling_parent = std::make_pair(parent.first, parent.second+1);
    assert(pager_->get_page_type(right_sibling) == PAGER_NODE_PAGE);
  } else if (parent.second+1 == parent_npm.num_cells_) {
    right_sibling = parent_npm.right_child_;
    right_sibling_parent = std::make_pair(parent.first, parent.second+1);
    assert(pager_->get_page_type(right_sibling) == PAGER_NODE_PAGE);
  }

  if (borrow_node_sibling_cell(curr_pgno, left_sibling, left_sibling_parent, true))
    return true;
  return borrow_node_sibling_cell(curr_pgno, right_sibling, right_sibling_parent, false);
}

PageNumber BTreeCursor::merge_with_node_sibling(
  PageNumber curr_pgno, BTreeCursorNode parent
) {
  assert(pager_ != nullptr);
  assert(pager_->get_page_type(curr_pgno) == PAGER_NODE_PAGE);
  assert(pager_->get_page_type(parent.first) == PAGER_NODE_PAGE);

  NodePageManager parent_npm(parent.first, pager_->db_file_ptr_);
  assert(parent_npm.num_cells_ == parent_npm.cells_.size());
  assert(parent.second <= parent_npm.num_cells_);

  PageNumber sibling_pgno;
  NodeCell_t pulldown_cell;
  PageNumber left_pgno, right_pgno;
  bool fix_right_child;

  if (parent.second == parent_npm.num_cells_) {
    sibling_pgno    = parent_npm.cells_[parent.second-1].left_child;
    pulldown_cell   = parent_npm.cells_[parent.second-1];
    left_pgno       = sibling_pgno;
    right_pgno      = curr_pgno;
    fix_right_child = true;
  } else {
    bool sibling_is_right_child = (parent.second == parent_npm.num_cells_ - 1);
    pulldown_cell   = parent_npm.cells_[parent.second];
    sibling_pgno    = sibling_is_right_child
      ? parent_npm.right_child_
      : parent_npm.cells_[parent.second+1].left_child;
    left_pgno       = curr_pgno;
    right_pgno      = sibling_pgno;
    fix_right_child = sibling_is_right_child;
  }

  NodePageManager left_npm(left_pgno, pager_->db_file_ptr_);
  NodePageManager right_npm(right_pgno, pager_->db_file_ptr_);
  assert(left_npm.num_cells_ + right_npm.num_cells_ + 1 <= config_.node_max_cells);

  for (auto& cell : left_npm.cells_) {
    right_npm.insert_node_cell(cell.key);
    right_npm.set_cell_left_child(cell.key, cell.left_child);
  }
  right_npm.insert_node_cell(pulldown_cell.key);
  right_npm.set_cell_left_child(pulldown_cell.key, left_npm.right_child_);

  parent_npm.delete_node_cell(pulldown_cell.key);
  if (fix_right_child)
    parent_npm.set_node_right_child(right_pgno);

  pager_->insert_freelist(left_pgno);
  return right_pgno;
}

void BTreeCursor::balance_node(PageNumber node_pgno) {
  assert(pager_ != nullptr);
  assert(pager_->get_page_type(node_pgno) == PAGER_NODE_PAGE);

  NodePageManager curr_npm(node_pgno, pager_->db_file_ptr_);
  assert(curr_npm.num_cells_ < config_.node_min_cells);

  assert(cursor_.size() > 0);
  BTreeCursorNode parent = cursor_.top();
  cursor_.pop();

  bool borrowed_from_sibling = borrow_from_node_sibling(
    parent, node_pgno
  );
  if (borrowed_from_sibling) return;

  PageNumber merged_pgno = merge_with_node_sibling(node_pgno, parent);
  NodePageManager parent_npm(parent.first, pager_->db_file_ptr_);
  if (root_pgno_ == parent.first && parent_npm.num_cells_ == 0) {
    pager_->insert_freelist(root_pgno_);
    set_root_pgno(merged_pgno);
  } else if (root_pgno_ != parent.first && parent_npm.num_cells_ < config_.node_min_cells)
    balance_node(parent.first);
}

void BTreeCursor::merge_with_leaf_sibling(BTreeCursorNode parent, PageNumber curr_pgno) {
  assert(parent.first != NULL_PAGE);
  assert(pager_ != nullptr);
  assert(pager_->get_page_type(parent.first) == PAGER_NODE_PAGE);
  assert(pager_->get_page_type(curr_pgno) == PAGER_LEAF_PAGE);

  NodePageManager parent_npm(parent.first, pager_->db_file_ptr_);
  assert(parent_npm.num_cells_ == parent_npm.cells_.size());
  assert(parent.second <= parent_npm.num_cells_);
  assert(
    (parent.second == parent_npm.num_cells_ && parent_npm.right_child_ == curr_pgno)
    || (parent.second < parent_npm.num_cells_ && parent_npm.cells_[parent.second].left_child == curr_pgno)
  );

  PageNumber sibling_pgno = NULL_PAGE;
  DefaultPagerKey parent_key_to_delete;
  PageNumber left_pgno, right_pgno;

  if (parent.second == parent_npm.num_cells_) {
    assert(parent_npm.right_child_ == curr_pgno);
    sibling_pgno = parent_npm.cells_[parent.second-1].left_child;
    parent_key_to_delete = parent_npm.cells_[parent.second-1].key;
    left_pgno = sibling_pgno;
    right_pgno = curr_pgno;
  } else {
    assert(parent_npm.cells_[parent.second].left_child == curr_pgno);
    sibling_pgno = (parent.second == parent_npm.num_cells_ - 1)
      ? parent_npm.right_child_
      : parent_npm.cells_[parent.second+1].left_child;
    parent_key_to_delete = parent_npm.cells_[parent.second].key;
    left_pgno = curr_pgno;
    right_pgno = sibling_pgno;
  }

  assert(pager_->get_page_type(sibling_pgno) == PAGER_LEAF_PAGE);
  LeafPageManager left_lpm(left_pgno, pager_->db_file_ptr_, pager_);
  LeafPageManager right_lpm(right_pgno, pager_->db_file_ptr_, pager_);

  assert(left_lpm.num_cells_ + right_lpm.num_cells_ <= config_.leaf_max_cells);
  for (auto& cell : left_lpm.cells_)
    right_lpm.write_cell(cell);

  assert(pager_->get_page_type(sibling_pgno) == PAGER_LEAF_PAGE);

  parent_npm.delete_node_cell(parent_key_to_delete);

  LeafPageManager left_lpm_ptr(left_pgno, pager_->db_file_ptr_, pager_);
  LeafPageManager right_lpm_ptr(right_pgno, pager_->db_file_ptr_, pager_);
  PageNumber left_prev_page = left_lpm_ptr.prev_page();

  right_lpm_ptr.set_prev_page(left_prev_page);
  if (left_prev_page != NULL_PAGE) {
    assert(pager_->get_page_type(left_prev_page) == PAGER_LEAF_PAGE);
    LeafPageManager left_prev_lpm(left_prev_page, pager_->db_file_ptr_, pager_);
    assert(left_prev_lpm.next_page() == left_pgno);
    left_prev_lpm.set_next_page(right_pgno);
  }

  pager_->insert_freelist(left_pgno);
  if (parent_npm.num_cells_ < config_.node_min_cells)
    balance_node(parent.first);
}

void BTreeCursor::set_root_pgno(PageNumber new_root_pgno) {
  // TODO(andrew): update this function later to do things
  // like update related indices + the master schema table
  root_pgno_ = new_root_pgno;
}