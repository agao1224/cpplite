#include <cstdint>
#include <cstddef>
#include <vector>
#include <optional>

#include "shared.h"
#include "pager/base_page.h"
#include "vfs/osfile.h"

#pragma once

// NOTE(andrew): probably rename this to 'BtreePageHeader'
// Structurally we don't differentiate between node, leaf, or root
typedef struct PagerNodePageHeader : public PagerBasePageHeader {
  uint16_t free_start;
  uint16_t free_end;
  uint16_t total_bytes_free;

  PagerNodePageHeader(
    uint32_t checksum_,
    PagerPageType page_type_,
    uint16_t free_start_,
    uint16_t free_end_,
    uint16_t total_bytes_free_
  ) : 
    PagerBasePageHeader(checksum_, page_type_),
    free_start(free_start_),
    free_end(free_end_),
    total_bytes_free(total_bytes_free_) {}

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

  void from_bytes(std::vector<std::byte> payload) {
    if (payload.size() < sizeof(PagerNodePageHeader))
      throw std::runtime_error("[PagerNodePageHeader] Payload too small");
    std::memcpy(static_cast<void*>(this), payload.data(), sizeof(PagerNodePageHeader));
  }
} PagerNodePageHeader_t;

class NodePageManager: public BasePageManager {
  private:
    PageNumber _create_overflow_pages(std::vector<std::byte> payload);

  public:
    NodePageManager(PageNumber pgno, std::shared_ptr<OsFile> db_file_ptr);
    ~NodePageManager();

    // NOTE(andrew): Node pages are implemented as slotted pages
    // See https://siemens.blog/posts/database-page-layout/
    uint16_t free_start_;
    uint16_t free_end_;
    uint16_t total_bytes_free_;

    std::vector<PagerCell_t> cells_;

    bool insert_cell(uint32_t key, std::vector<std::byte> cell_data);
    std::optional<std::vector<std::byte>> find_cell(uint32_t key);
    size_t get_free_space();
};