#include <cstdint>
#include <cstddef>
#include <vector>

#include "shared.h"
#include "vfs/osfile.h"

#pragma once

#define NULL_PAGE 0

typedef uint64_t PageNumber;

typedef enum { 
  PAGER_FREE_PAGE,
  PAGER_FIRST_PAGE,
  PAGER_NODE_PAGE,
  PAGER_LEAF_PAGE,
  PAGER_OVERFLOW_PAGE,
} PagerPageType;

typedef struct PagerBasePageHeader {
  uint32_t checksum;
  PagerPageType page_type;

  PagerBasePageHeader(uint32_t checksum_, PagerPageType page_type_) :
    checksum(checksum_), page_type(page_type_) {}

  PagerBasePageHeader(std::vector<std::byte> payload) {
    if (payload.size() < sizeof(PagerBasePageHeader))
      throw std::runtime_error("[PagerBasePageHeader] Payload too small");
    std::memcpy(static_cast<void*>(this), payload.data(), sizeof(PagerBasePageHeader));
  }

} PagerBasePageHeader_t;

class BasePageManager {
  public:
    virtual ~BasePageManager() = default;

    std::shared_ptr<OsFile> db_file_ptr_ = nullptr;
    uint32_t checksum_;
    PagerPageType page_type_;
    PageNumber pgno_;
    std::vector<std::byte> data_;

    void set_data(std::vector<std::byte> buffer, size_t num_bytes, PageNumber page_num);
};