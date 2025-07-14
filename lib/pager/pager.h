#include <cstdint>
#include <cstddef>
#include <vector> 

#include "../shared.h"
#include "../vfs/osfile.h"

// NOTE(andrew): technically free page and overflow page are 
// also node pages, but we PAGER_NODE_PAGE to indicate
// in-use, non-overflow pages
typedef enum { 
  PAGER_FREE_PAGE, 
  PAGER_FIRST_PAGE, 
  PAGER_NODE_PAGE, 
  PAGER_ROOT_PAGE, 
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

} PagerBasePageHeader_t;

typedef struct PagerFreePageHeader : public PagerBasePageHeader {
  PageNumber next_free_page;

  PagerFreePageHeader(uint32_t checksum_, PagerPageType page_type_, PageNumber next_free_page_) :
    PagerBasePageHeader(checksum_, page_type_), next_free_page(next_free_page_) {}

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
    std::vector<std::byte> data_;

    void set_data(std::vector<std::byte> buffer, size_t num_bytes, PageNumber page_num);
};

class FirstPageManager: public BasePageManager {
  public:
    FirstPageManager(std::shared_ptr<OsFile> db_file_ptr);
    ~FirstPageManager();

    uint64_t num_pages_;
    PageNumber free_page_head_;

    void set_num_pages(uint64_t num_pages);
    void set_free_page_head(PageNumber pgno);
    PageNumber get_free_page_head();
};

class NodePageManager: public BasePageManager {
  public:
    NodePageManager(PageNumber page_num, std::shared_ptr<OsFile> db_file_ptr);
    ~NodePageManager();
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
    BasePageManager page_manager_;

  public:
  // https://medium.com/technology-in-essence/how-sqlite-database-works-b10ac80e4f07
    Pager() = default;
    ~Pager() = default;

    void open(std::string db_filename);
    void seek_page(PageNumber page_num);

    PageNumber create_free_page();
    PageNumber create_overflow_page(std::vector<std::byte> payload);
    PageNumber create_node_page();

    // Methods for setting data
    void write_data(std::vector<std::byte> buffer);
    void set_num_pages(PageNumber new_num_pages);
};
