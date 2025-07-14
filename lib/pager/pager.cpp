#include <memory>
#include <cassert>

#include "pager.h"

void Pager::open(std::string db_filename) {
  std::shared_ptr<OsFile> db_file_ptr = std::make_shared<OsFile>(db_filename);
  db_file_ptr_ = std::move(db_file_ptr);

  curr_page_num_ = 1;
  page_manager_ = FirstPageManager(db_file_ptr_);
}

void Pager::seek_page(PageNumber page_num) {
  if (page_num == curr_page_num_)
    return;

  if (page_num == 1)
    page_manager_ = FirstPageManager(db_file_ptr_);  
  else
    page_manager_ = NodePageManager(page_num, db_file_ptr_);
}

PageNumber Pager::create_free_page() {
  assert(db_file_ptr_ != nullptr);

  OsFile db_file = *db_file_ptr_;
  FirstPageManager fp_manager(std::move(db_file_ptr_)); 
  
  total_num_pages_++;

  PageNumber new_free_pgno = total_num_pages_;
  PageNumber old_free_page_head = fp_manager.get_free_page_head();
  PagerFreePageHeader_t free_page_header = PagerFreePageHeader(
    CHECKSUM,
    PAGER_FREE_PAGE,
    old_free_page_head
  );

  db_file.os_append(free_page_header.to_bytes(), PAGE_SIZE);
  fp_manager.set_free_page_head(new_free_pgno);

  return new_free_pgno;
}

PageNumber Pager::create_overflow_page(
  std::vector<std::byte> payload
) {
  assert(db_file_ptr_ != nullptr);

  OsFile db_file = *db_file_ptr_;
  total_num_pages_++;

  PageNumber new_pgno = total_num_pages_;
  PagerOverflowPageHeader_t overflow_page_header = PagerOverflowPageHeader(
    CHECKSUM,
    PAGER_OVERFLOW_PAGE,
    0
  );

  std::vector<std::byte> overflow_page_data = overflow_page_header.to_bytes();
  overflow_page_data.insert(
    overflow_page_data.begin(),
    payload.begin(),
    payload.end()
  );

  db_file.os_append(overflow_page_data, PAGE_SIZE);
  return new_pgno;
}

PageNumber Pager::create_node_page() {
  assert (db_file_ptr_ != nullptr);

  OsFile db_file = *db_file_ptr_;

  PageNumber new_pgno = total_num_pages_ + 1;
  PagerNodePageHeader_t node_page_header = PagerNodePageHeader(
    CHECKSUM,
    PAGER_NODE_PAGE
  );

  db_file.os_append(node_page_header.to_bytes(), PAGE_SIZE);
  total_num_pages_ = new_pgno;
  return new_pgno;
}

void Pager::write_data(std::vector<std::byte> buffer) {
  // NOTE(andrew): we don't check if db_file_ptr_ is null
  // since page_manager_ptr_ has ownership
  assert(1 <= curr_page_num_ && curr_page_num_ <= total_num_pages_ &&
         page_open_);

  auto& page_manager = page_manager_;
  page_manager.set_data(buffer, sizeof(buffer), curr_page_num_);
}

// NOTE(andrew): not sure if this is safe, have to look
// into how resource ownership works in cpp
void Pager::set_num_pages(PageNumber new_num_pages) {
  assert(page_open_);

  FirstPageManager tmp_manager(db_file_ptr_);
  tmp_manager.set_num_pages(new_num_pages);

  if (curr_page_num_ == 1)
    page_manager_ = FirstPageManager(db_file_ptr_);
  else
    page_manager_ = NodePageManager(curr_page_num_, db_file_ptr_);
}
