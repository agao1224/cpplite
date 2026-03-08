#include <cstdint>
#include <cstddef>
#include <vector>

#include "shared.h"
#include "pager/base_page.h"
#include "vfs/osfile.h"

#pragma once

typedef struct PagerOverflowPageHeader : public PagerBasePageHeader {
  PageNumber next_overflow_page;
  uint32_t num_bytes;

  PagerOverflowPageHeader(
    uint32_t checksum_,
    PagerPageType page_type_,
    PageNumber next_overflow_page_,
    uint32_t num_bytes_
  ) :
    PagerBasePageHeader(checksum_, page_type_),
    next_overflow_page(next_overflow_page_),
    num_bytes(num_bytes_)
    {}

  PagerOverflowPageHeader(std::vector<std::byte> payload)
    : PagerBasePageHeader(CHECKSUM, PAGER_OVERFLOW_PAGE)
  {
    if (payload.size() < sizeof(PagerOverflowPageHeader))
      throw std::runtime_error("[PagerOverflowPageHeader] Payload too small");
    std::memcpy(static_cast<void*>(this), payload.data(), sizeof(PagerOverflowPageHeader));
  }

  std::vector<std::byte> to_bytes() const {
    std::vector<std::byte> buffer(sizeof(*this));
    std::memcpy(buffer.data(), static_cast<const void*>(this), sizeof(*this));
    return buffer;
  }

  void from_bytes(std::vector<std::byte> payload) {
    if (payload.size() < sizeof(PagerOverflowPageHeader))
      throw std::runtime_error("[PagerOverflowPageHeader] Payload too small");
    std::memcpy(static_cast<void*>(this), payload.data(), sizeof(PagerOverflowPageHeader));
  }
} PagerOverflowPageHeader_t;


class OverflowPageManager: public BasePageManager {
  public:
    OverflowPageManager(PageNumber pgno, std::shared_ptr<OsFile> db_file_ptr);
    ~OverflowPageManager();

    PageNumber next_overflow_page_;
    uint32_t num_bytes_;

    PageNumber get_next_overflow_page();
    uint32_t get_num_bytes();
    void set_next_overflow_page(PageNumber pgno);
    std::vector<std::byte> get_overflow_content();
};

