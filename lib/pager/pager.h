#include <cstddef>
#include <vector>

#include "shared.h"
#include "base_page.h"
#include "pager/overflow_page/overflow_page.h"
#include "pager/free_page/free_page.h"
#include "pager/first_page/first_page.h"
#include "pager/leaf_page/leaf_page.h"
#include "pager/node_page/node_page.h"
#include "vfs/osfile.h"

#pragma once


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

    // NOTE(andrew):
    // Writes payload across one or more chained overflow pages, reusing free
    // pages first then appending new ones. Returns the first PageNumber in the chain.
    PageNumber _write_overflow_chain(std::vector<std::byte> payload);

  public:
  // https://medium.com/technology-in-essence/how-sqlite-database-works-b10ac80e4f07
    std::optional<
      std::variant<
        FirstPageManager,
        NodePageManager,
        LeafPageManager,
        BasePageManager,
        FreePageManager,
        OverflowPageManager
      >
    > page_manager_;
    Pager(std::string db_filename);
    ~Pager() = default;

    void seek_page(PageNumber pgno);

    PageNumber create_page(PagerPageType page_type);
    // NOTE(andrew): create_page overload is specifically for creating overflow page
    PageNumber create_page(PagerPageType page_type, std::vector<std::byte> payload);

    void write_data(std::vector<std::byte> buffer);
    void set_num_pages(PageNumber new_num_pages);

    void insert_freelist(PageNumber pgno);
    PageNumber pop_freelist();
    PageNumber peek_freelist();
};
