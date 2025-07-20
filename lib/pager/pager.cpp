#include <memory>
#include <cassert>
#include <stdexcept>

#include "pager.h"

Pager::Pager(std::string db_filename) {
  PagerFirstPageHeader_t first_page_header(
    CHECKSUM,
    PAGER_FIRST_PAGE,
    1,
    NULL_PAGE
  );

  std::vector<std::byte> first_page_content(sizeof(PagerFirstPageHeader_t));
  std::memcpy(
    first_page_content.data(),
    &first_page_header,
    sizeof(PagerFirstPageHeader_t)
  );

  db_file_ptr_ = std::make_shared<OsFile>(db_filename);

  OsFile os_file = *db_file_ptr_;
  os_file.os_open();
  os_file.os_seek(0);
  os_file.os_write(
    first_page_content,
    PAGE_SIZE
  );
  os_file.os_close();
}

void Pager::seek_page(PageNumber pgno) {
  assert(db_file_ptr_ != nullptr);

  if (pgno == PAGER_FIRST_PAGE) {
    FirstPageManager fpm(db_file_ptr_);
    page_manager_ = std::make_optional<FirstPageManager>(fpm);
    return;
  }

  std::vector<std::byte> page_content(PAGE_SIZE);
  OsFile db_file = *db_file_ptr_;
  db_file.os_open();
  db_file.os_seek((pgno-1)*PAGE_SIZE);
  db_file.os_read(page_content, PAGE_SIZE);
  db_file.os_close();

  PagerBasePageHeader_t page_header(page_content);
  switch (page_header.page_type) {
    case PAGER_FIRST_PAGE: {
      FirstPageManager fpm(db_file_ptr_);
      page_manager_ = std::make_optional<FirstPageManager>(fpm);
      break;
    }
    case PAGER_NODE_PAGE: {
      NodePageManager npm(pgno, db_file_ptr_);
      page_manager_ = std::make_optional<NodePageManager>(npm);
      break;
    }
    case PAGER_FREE_PAGE: {
      FreePageManager fpm(pgno, db_file_ptr_);
      page_manager_ = std::make_optional<FreePageManager>(fpm);
      break;
    }
    case PAGER_OVERFLOW_PAGE: {
      OverflowPageManager opm(pgno, db_file_ptr_);
      page_manager_ = std::make_optional<OverflowPageManager>(opm);
      break;
    }
    default:
      throw std::runtime_error("[Pager:seek_page]: Attempted to seek invalid page type");
  }
}

PageNumber Pager::create_free_page() {
  assert(db_file_ptr_ != nullptr);

  FirstPageManager fp_manager(db_file_ptr_); 

  total_num_pages_++;

  PageNumber new_free_pgno = total_num_pages_;
  PageNumber old_free_page_head = fp_manager.get_free_page_head();
  PagerFreePageHeader_t free_page_header = PagerFreePageHeader(
    CHECKSUM,
    PAGER_FREE_PAGE,
    old_free_page_head
  );

  OsFile db_file = *db_file_ptr_;
  db_file.os_open();
  db_file.os_append(free_page_header.to_bytes(), PAGE_SIZE);
  db_file.os_close();
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

PageNumber Pager::get_free_page_head() {
  assert(db_file_ptr_ != nullptr);

  std::vector<std::byte> first_page_content(PAGE_SIZE);
  OsFile db_file = *db_file_ptr_;
  db_file.os_open();
  db_file.os_seek(0);
  db_file.os_read(first_page_content, PAGE_SIZE);
  db_file.os_close();

  FirstPageManager fpm(db_file_ptr_);
  return fpm.get_free_page_head();
}

PageNumber Pager::create_node_page() {
  assert (db_file_ptr_ != nullptr);

  OsFile db_file = *db_file_ptr_;
  db_file.os_open();

  PageNumber new_pgno = total_num_pages_ + 1;
  PagerNodePageHeader_t node_page_header(
    CHECKSUM,
    PAGER_NODE_PAGE
  );

  db_file.os_append(node_page_header.to_bytes(), PAGE_SIZE);
  total_num_pages_ = new_pgno;
  db_file.os_close();
  return new_pgno;
}

void Pager::write_data(std::vector<std::byte> buffer) {
  // NOTE(andrew): we don't check if db_file_ptr_ is null
  // since page_manager_ptr_ has ownership
  assert(1 <= curr_page_num_ && curr_page_num_ <= total_num_pages_ &&
         page_open_);
  assert (page_manager_.has_value());

  auto& page_manager = std::get<BasePageManager>(page_manager_.value());
  page_manager.set_data(buffer, sizeof(buffer), curr_page_num_);
}

void Pager::set_num_pages(PageNumber new_num_pages) {
  assert(db_file_ptr_ != nullptr);

  FirstPageManager fpm;
  if (!page_manager_.has_value()) {
    fpm = FirstPageManager(db_file_ptr_);
  } else {
    fpm = std::get<FirstPageManager>(page_manager_.value());
  }

  fpm.set_num_pages(new_num_pages);
}
