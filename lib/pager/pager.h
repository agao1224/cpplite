#include <cstdint>
#include <cstddef>
#include <vector>
#include <optional>

#include "shared.h"
#include "vfs/osfile.h"

#pragma once

#define NULL_PAGE 0

// NOTE(andrew): technically free page and overflow page are 
// also node pages, but we PAGER_NODE_PAGE to indicate
// in-use, non-overflow pages
typedef enum { 
  PAGER_FREE_PAGE, 
  PAGER_FIRST_PAGE, 
  PAGER_NODE_PAGE, 
  PAGER_OVERFLOW_PAGE,
} PagerPageType;

typedef uint64_t PageNumber;

/**
 * @brief PagerCell specifies
 * 
 */
typedef struct PagerCell {
  uint16_t offset;
  uint16_t size;
  uint32_t key;

  PageNumber left_child;
  uint16_t free_start;
  uint16_t free_end;
} PagerCell_t;

/**
 * @brief BasePageHeader with shared fields across
 * all page types
 *
 */
typedef struct PagerBasePageHeader {
  uint32_t checksum;
  PagerPageType page_type;

  PagerBasePageHeader(uint32_t checksum_, PagerPageType page_type_) :
    checksum(checksum_), page_type(page_type_) {}

  PagerBasePageHeader(std::vector<std::byte> payload) {
    if (payload.size() < sizeof(PagerBasePageHeader))
      throw std::runtime_error("[PagerNodePageHeader] Payload too small");
    std::memcpy(static_cast<void*>(this), payload.data(), sizeof(PagerBasePageHeader));
  }

} PagerBasePageHeader_t;

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

typedef struct PagerOverflowPageHeader : public PagerBasePageHeader {
  PageNumber next_overflow_page;

  PagerOverflowPageHeader(uint32_t checksum_, PagerPageType page_type_, PageNumber next_overflow_page_) :
    PagerBasePageHeader(checksum_, page_type_), next_overflow_page(next_overflow_page_) {}

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

// TODO(andrew): probably rename this to 'BtreePageHeader'
// Structurally we don't differentiate between node, leaf, or root
typedef struct PagerNodePageHeader : public PagerBasePageHeader {

  PagerNodePageHeader(uint32_t checksum_, PagerPageType page_type_) :
    PagerBasePageHeader(checksum_, page_type_) {}

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

/**
 * @brief We have both next_free_page and next_overflow_page
 * b/c the master table is stored in data, and could possibly
 * overflow (large number of tables, metadata, etc)
 *
 * NOTE(andrew): The user shouldn't ever create the first page
 */
typedef struct PagerFirstPageHeader : public PagerBasePageHeader {
  uint64_t num_pages;
  uint64_t free_page_head;

  PagerFirstPageHeader(uint32_t checksum_, PagerPageType page_type_, uint64_t num_pages_, PageNumber free_page_head_) :
    PagerBasePageHeader(checksum_, page_type_), num_pages(num_pages_), free_page_head(free_page_head_) {}

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

class FirstPageManager: public BasePageManager {
  public:
    FirstPageManager(std::shared_ptr<OsFile> db_file_ptr);
    FirstPageManager();
    ~FirstPageManager();

    uint64_t num_pages_;
    PageNumber free_page_head_ = NULL_PAGE;

    void set_num_pages(uint64_t num_pages);
    void set_free_page_head(PageNumber pgno);
    PageNumber get_free_page_head();
};

class NodePageManager: public BasePageManager {
  public:
    NodePageManager(PageNumber pgno, std::shared_ptr<OsFile> db_file_ptr);
    ~NodePageManager();
};

class FreePageManager: public BasePageManager {
  public:
    FreePageManager(PageNumber pgno, std::shared_ptr<OsFile> db_file_ptr);
    ~FreePageManager();
  
    PageNumber next_free_page_;

    PageNumber get_next_free_page();
    void set_next_free_page(PageNumber pgno);
};

class OverflowPageManager: public BasePageManager {
  public:
    OverflowPageManager(PageNumber pgno, std::shared_ptr<OsFile> db_file_ptr);
    ~OverflowPageManager();

    PageNumber next_overflow_page_;

    PageNumber get_next_overflow_page();
    void set_next_overflow_page(PageNumber pgno);
};

class Pager {
  private:
    /**
     * @brief 
     * Interface to interact with OS filesystem
     *
     * By default we set curr_page_num_ to the first page,
     * and we assume that it will always exist (no need to
     * create upon init)
     */
    std::shared_ptr<OsFile> db_file_ptr_ = nullptr;
    bool page_open_ = false;
    PageNumber curr_page_num_ = 1;
    PageNumber total_num_pages_ = 1;

  public:
  // https://medium.com/technology-in-essence/how-sqlite-database-works-b10ac80e4f07
    std::optional<
      std::variant<
        FirstPageManager,
        NodePageManager,
        BasePageManager,
        FreePageManager,
        OverflowPageManager
      >
    > page_manager_;
    Pager(std::string db_filename);
    ~Pager() = default;

    void seek_page(PageNumber pgno);

    // TODO(andrew): consolidate this all into a single method
    // with overloads in order to create different types of pages
    PageNumber create_free_page();
    PageNumber create_overflow_page(std::vector<std::byte> payload);
    PageNumber create_node_page();

    // Methods for setting data
    void write_data(std::vector<std::byte> buffer);
    void set_num_pages(PageNumber new_num_pages);

    // Page freelist
    void insert_freelist(PageNumber pgno);
    PageNumber pop_freelist();
    PageNumber peek_freelist();
};
