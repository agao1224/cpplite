#include <vector>

#include "shared.h"
#include "pager/base_page.h"

#pragma once

typedef struct PagerFirstPageHeader : public PagerBasePageHeader {
  uint64_t num_pages;
  uint64_t free_page_head;
  DefaultPagerKey next_oid;

  PagerFirstPageHeader(uint32_t checksum_, PagerPageType page_type_, uint64_t num_pages_, PageNumber free_page_head_, DefaultPagerKey next_oid_) :
    PagerBasePageHeader(checksum_, page_type_), num_pages(num_pages_), free_page_head(free_page_head_), next_oid(next_oid_) {}

  PagerFirstPageHeader(std::vector<std::byte> payload)
    : PagerBasePageHeader(CHECKSUM, PAGER_FIRST_PAGE) // NOTE(andrew): tmp values, overwritten by memcpy
  {
    if (payload.size() < sizeof(PagerFirstPageHeader))
      throw std::runtime_error("[PagerFirstPageHeader] Payload too small");
    std::memcpy(static_cast<void*>(this), payload.data(), sizeof(PagerFirstPageHeader));
  }

  std::vector<std::byte> to_bytes() const {
    std::vector<std::byte> buffer(sizeof(*this));
    std::memcpy(buffer.data(), static_cast<const void*>(this), sizeof(*this));
    return buffer;
  }

  void from_bytes(std::vector<std::byte> payload) {
    if (payload.size() < sizeof(PagerFirstPageHeader))
      throw std::runtime_error("[PagerFirstPageHeader] Payload too small");
    std::memcpy(static_cast<void*>(this), payload.data(), sizeof(PagerFirstPageHeader));
  }
} PagerFirstPageHeader_t;

class FirstPageManager: public BasePageManager {
  public:
    FirstPageManager(std::shared_ptr<OsFile> db_file_ptr);
    FirstPageManager();
    ~FirstPageManager();

    uint64_t num_pages_;
    PageNumber free_page_head_ = NULL_PAGE;
    DefaultPagerKey next_oid_ = DefaultPagerKey(1);

    void set_num_pages(uint64_t num_pages);
    void set_free_page_head(PageNumber pgno);
    PageNumber get_free_page_head();
    PageNumber get_next_free_page(PageNumber pgno);
    PageNumber create_free_page();
    DefaultPagerKey get_next_oid();
    void set_next_oid(DefaultPagerKey oid);
};