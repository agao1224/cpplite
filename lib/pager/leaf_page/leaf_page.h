#include <cstdint>
#include <cstddef>
#include <vector>
#include <optional>

#include "shared.h"
#include "pager/base_page.h"
#include "vfs/osfile.h"

#pragma once

typedef struct LeafCell {
  uint32_t payload_size;
  DefaultPagerKey key;

  PageNumber record_page;
  std::vector<std::byte> to_bytes() const {
    std::vector<std::byte> buffer(sizeof(*this));
    std::memcpy(buffer.data(), static_cast<const void*>(this), sizeof(*this));
    return buffer;
  }
} LeafCell_t;

typedef struct PagerLeafPageHeader : public PagerBasePageHeader {
  size_t num_cells;
  size_t total_bytes_free;
  PageNumber next_page;
  PageNumber prev_page;

  PagerLeafPageHeader(
    uint32_t checksum_,
    PagerPageType page_type_,
    size_t num_cells_,
    uint16_t total_bytes_free_,
    PageNumber next_page_,
    PageNumber prev_page_    
  ) :
    PagerBasePageHeader(checksum_, page_type_),
    num_cells(num_cells_),
    total_bytes_free(total_bytes_free_),
    next_page(next_page_),
    prev_page(prev_page_)
    {}

  PagerLeafPageHeader(std::vector<std::byte> payload)
    : PagerBasePageHeader(CHECKSUM, PAGER_LEAF_PAGE)
  {
    if (payload.size() < sizeof(PagerLeafPageHeader))
      throw std::runtime_error("[PagerLeafPageHeader] Payload too small"); 
    std::memcpy(static_cast<void*>(this), payload.data(), sizeof(PagerLeafPageHeader));
  }

  std::vector<std::byte> to_bytes() const {
    std::vector<std::byte> buffer(sizeof(*this));
    std::memcpy(buffer.data(), static_cast<const void*>(this), sizeof(*this));
    return buffer;
  }
} PagerLeafPageHeader_t; 

class Pager;

class LeafPageManager: public BasePageManager {
  private:
    Pager* pager_;
    void _set_leaf_page_pointers(PageNumber pgno, bool use_prev_page);

  public:
    LeafPageManager(PageNumber pgno, std::shared_ptr<OsFile> db_file_ptr, Pager* pager);
    ~LeafPageManager();

    std::vector<LeafCell_t> cells_;
    size_t num_cells_;
    uint16_t total_bytes_free_;
    PageNumber prev_page_;
    PageNumber next_page_;

    bool insert_cell(DefaultPagerKey key, std::vector<std::byte> cell_data);
    bool write_cell(LeafCell_t cell);
    std::optional<std::vector<std::byte>> get_payload(DefaultPagerKey key);
    size_t get_free_space();

    PageNumber prev_page();
    PageNumber next_page();
    void set_prev_page(PageNumber pgno);
    void set_next_page(PageNumber pgno);
};

const size_t LEAF_MAX_CELLS = (PAGE_SIZE - sizeof(PagerLeafPageHeader_t)) / sizeof(LeafCell_t);
const size_t LEAF_MIN_CELLS = LEAF_MAX_CELLS / 2;