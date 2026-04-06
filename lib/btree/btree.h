#include <map>
#include <stack>
#include <utility>

#include "shared.h"
#include "pager/pager.h"

#pragma once

typedef std::pair<PageNumber, size_t> BTreeCursorStackElt;
typedef std::stack<BTreeCursorStackElt> BTreeCursorStack;

struct BTreeConfig {
  size_t node_max_cells = NODE_MAX_CELLS;
  size_t node_min_cells = NODE_MIN_CELLS;
  size_t leaf_max_cells = LEAF_MAX_CELLS;
  size_t leaf_min_cells = LEAF_MIN_CELLS;
};

struct SplitNodeRes {
  PageNumber l;
  PageNumber r;
  NodeCell_t l_parent;
  std::optional<NodeCell_t> r_parent;
};

struct SplitCellRes {
  std::vector<DefaultPagerKey> left_keys;
  std::vector<DefaultPagerKey> right_keys;
  std::map<
    DefaultPagerKey,
    std::variant<NodeCell_t, LeafCell_t>
  > cell_map;
};

class BTreeCursor {
  private:
    std::stack<std::pair<PageNumber, size_t>> cursor_;
    Pager* pager_;
    PageNumber root_pgno_;
    BTreeConfig config_;

    SplitCellRes split_cell(
      PageNumber pgno,
      std::variant<NodeCell_t, LeafCell_t> cell
    );
    NodeCell_t assign_children_nodes(
      PageNumber left_node,
      PageNumber right_node,
      BTreeCursorStackElt parent,
      DefaultPagerKey separator_key
    );
    void split_cursor_leaf(LeafCell_t new_cell);
    void split_cursor_node(NodeCell_t new_cell);

  public:
    BTreeCursor(Pager* pager, PageNumber root_pgno, BTreeConfig config = {});
    ~BTreeCursor();

    BTreeCursorStack get_cursor_stack() const { return cursor_; }
    Pager* get_pager() const { return pager_; }
    PageNumber get_root_pgno() const { return root_pgno_; }

    void move_to_first();
    void move_to_last();
    bool move_to_key(DefaultPagerKey key);
    bool prev();
    bool next();

    DefaultPagerKey current_key() const;
    PageNumber current_pgno() const;
    PageNumber current_record_pgno() const;
    std::vector<std::byte> current_value() const;

    void insert(DefaultPagerKey key, std::vector<std::byte> value);
    bool remove();
};
