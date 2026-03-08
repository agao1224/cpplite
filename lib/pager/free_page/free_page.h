#include <cstdint>
#include <cstddef>
#include <vector>

#include "pager/base_page.h"

#pragma once

typedef struct PagerFreePageHeader : public PagerBasePageHeader {
  PageNumber next_free_page;

  PagerFreePageHeader(uint32_t checksum_, PagerPageType page_type_, PageNumber next_free_page_) :
    PagerBasePageHeader(checksum_, page_type_), next_free_page(next_free_page_) {}

  PagerFreePageHeader(std::vector<std::byte> payload) 
    : PagerBasePageHeader(CHECKSUM, PAGER_FREE_PAGE)
  {
    if (payload.size() < sizeof(PagerFreePageHeader))
      throw std::runtime_error("[PagerFreePageHeader] Payload too small");
    std::memcpy(static_cast<void*>(this), payload.data(), sizeof(PagerFreePageHeader));
  }

  std::vector<std::byte> to_bytes() const {
    std::vector<std::byte> buffer(sizeof(*this));
    std::memcpy(buffer.data(), static_cast<const void*>(this), sizeof(*this));
    return buffer;
  }
} PagerFreePageHeader_t;

class FreePageManager: public BasePageManager {
  public:
    FreePageManager(PageNumber pgno, std::shared_ptr<OsFile> db_file_ptr);
    ~FreePageManager();
  
    PageNumber next_free_page_;

    PageNumber get_next_free_page();
    void set_next_free_page(PageNumber pgno);
};
