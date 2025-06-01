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
  PAGER_OVERFLOW_PAGE,
} PagerPageType;

typedef uint64_t PageNumber;

/**
 * @brief Root pages and internal pages are identical
 * 
 */
typedef struct PagerNodePageHeader {
  uint32_t checksum;
  PagerPageType page_type;
} PagerNodePageHeader_t;

/**
 * @brief We have both next_free_page and next_overflow_page
 * b/c the master table is stored in data, and could possibly
 * overflow (large number of tables, metadata, etc)
 */
typedef struct PagerFirstPageHeader {
  uint32_t checksum;
  PagerPageType page_type;
  uint64_t num_pages;
} PagerFirstPageHeader_t;

class BasePageManager {
  public:
    virtual ~BasePageManager() = default;

    std::unique_ptr<OsFile> db_file_ptr_ = nullptr;
    uint32_t checksum_;
    PagerPageType page_type_;
    // TODO(andrew):
    // not sure if we necessarily need these to populate
    // these fields yet, only way we find out is by
    // inspecting the actual btree.
    // 
    // PageNumber next_free_page_;
    // PageNumber next_overflow_page_;
    std::vector<std::byte> data_;

    void set_data(std::vector<std::byte> buffer, size_t num_bytes);
};

class FirstPageManager: public BasePageManager {
  public:
    FirstPageManager(std::unique_ptr<OsFile> db_file_ptr);
    ~FirstPageManager();

    uint64_t num_pages_;

    void set_num_pages(uint64_t num_pages);
};

class NodePageManager: public BasePageManager {
  public:
    NodePageManager(PageNumber page_num, std::unique_ptr<OsFile> db_file_ptr);
    ~NodePageManager();
};

class Pager {
  private:
    /**
     * @brief 
     * Interface to interact with OS filesystem
     */
    std::unique_ptr<OsFile> db_file;
  
  public:
  // TODO(andrew):
  // Pager acts as the interface between btree
  // and VFS. what would we need here?
  //
  // I think we can keep the header structs and use them
  // to help marshalling the data in easier. then ultimately
  // use VFS and write + flush page to filesys
  //
  // https://medium.com/technology-in-essence/how-sqlite-database-works-b10ac80e4f07

};