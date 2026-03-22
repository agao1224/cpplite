#include <cstdint>
#include <cstddef>
#include <optional>
#include <vector>

#include "shared.h"
#include "pager/base_page.h"
#include "vfs/osfile.h"

#pragma once

typedef struct NodeCell {
  DefaultPagerKey key;
  PageNumber left_child;

  NodeCell() = default;
  NodeCell(DefaultPagerKey key_, PageNumber left_child_) :
    key(key_), left_child(left_child_) {}

  std::vector<std::byte> to_bytes() const {
    std::vector<std::byte> buffer(sizeof(*this));
    std::memcpy(buffer.data(), static_cast<const void*>(this), sizeof(*this));
    return buffer;
  }
} NodeCell_t;

// NOTE(andrew): probably rename this to 'BtreePageHeader'
// Structurally we don't differentiate between node, leaf, or root
typedef struct PagerNodePageHeader : public PagerBasePageHeader {
  size_t num_cells;
  size_t total_bytes_free;
  PageNumber right_child;

  PagerNodePageHeader(
    uint32_t checksum_,
    PagerPageType page_type_,
    size_t num_cells_,
    uint16_t total_bytes_free_,
    PageNumber right_child_
  ) : 
    PagerBasePageHeader(checksum_, page_type_),
    num_cells(num_cells_),
    total_bytes_free(total_bytes_free_),
    right_child(right_child_)
    {}

  PagerNodePageHeader(std::vector<std::byte> payload)
    : PagerBasePageHeader(CHECKSUM, PAGER_NODE_PAGE)
  {
    if (payload.size() < sizeof(PagerNodePageHeader))
      throw std::runtime_error("[PagerNodePageHeader] Payload too small");
    std::memcpy(static_cast<void*>(this), payload.data(), sizeof(PagerNodePageHeader));
  }

  std::vector<std::byte> to_bytes() const {
    std::vector<std::byte> buffer(sizeof(*this));
    std::memcpy(buffer.data(), static_cast<const void*>(this), sizeof(*this));
    return buffer;
  }
} PagerNodePageHeader_t;

class NodePageManager: public BasePageManager {
  private:
    std::optional<size_t> find_node_cell_idx(DefaultPagerKey key);

  public:
    NodePageManager(PageNumber pgno, std::shared_ptr<OsFile> db_file_ptr);
    ~NodePageManager();

    std::vector<NodeCell_t> cells_;
    size_t num_cells_;
    uint16_t total_bytes_free_;
    PageNumber right_child_;

    bool insert_node_cell(DefaultPagerKey key);
    void set_cell_left_child(DefaultPagerKey key, PageNumber left_child_pgno);
    void set_node_right_child(PageNumber right_child_pgno);
    size_t get_free_space();
};