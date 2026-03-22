#include <stack>
#include <utility>

#include "shared.h"
#include "pager/pager.h"

#pragma once

typedef std::stack<std::pair<PageNumber, size_t>> BTreeCursorStack;

class BTreeCursor {
  private:
    std::stack<std::pair<PageNumber, size_t>> cursor_;
    Pager* pager_;
    PageNumber root_pgno_;

  public:
    BTreeCursor(Pager* pager, PageNumber root_pgno);
    ~BTreeCursor();

    void move_to_first();
    void move_to_last();
    bool move_to_key(DefaultPagerKey key);
    bool next();
    bool prev();
    bool is_valid() const;

    DefaultPagerKey current_key() const;
    std::vector<std::byte> current_value() const;

    bool insert(DefaultPagerKey key, std::vector<std::byte> value);
    bool remove();
};
