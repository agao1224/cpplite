#include <cassert>
#include <cstring>
#include <memory>
#include <stdexcept>

#include "pager.h"
#include "pager/base_page.h"
#include "pager/first_page/first_page.h"
#include "pager/free_page/free_page.h"
#include "pager/leaf_page/leaf_page.h"
#include "pager/overflow_page/overflow_page.h"
#include "shared.h"

Pager::Pager(std::string db_filename) {
  PagerFirstPageHeader_t first_page_header(CHECKSUM, PAGER_FIRST_PAGE, 1,
                                           NULL_PAGE, DefaultPagerKey(1));

  std::vector<std::byte> first_page_content(sizeof(PagerFirstPageHeader_t));
  std::memcpy(first_page_content.data(), &first_page_header,
              sizeof(PagerFirstPageHeader_t));

  db_file_ptr_ = std::make_shared<OsFile>(db_filename);

  OsFile os_file = *db_file_ptr_;
  os_file.os_open();
  bool seek_ok = os_file.os_seek(0);
  if (!seek_ok)
    throw std::runtime_error("[Pager:Pager]: Failed to seek");

  os_file.os_write(first_page_content, PAGE_SIZE);
  os_file.os_close();
}

void Pager::seek_page(PageNumber pgno) {
  assert(db_file_ptr_ != nullptr);

  if (pgno == PAGER_FIRST_PAGE) {
    FirstPageManager fpm(db_file_ptr_);
    page_manager_ = fpm;
    return;
  }

  std::vector<std::byte> page_content(PAGE_SIZE);
  OsFile db_file = *db_file_ptr_;
  db_file.os_open();
  bool seek_ok = db_file.os_seek((pgno - 1) * PAGE_SIZE);
  if (!seek_ok)
    throw std::runtime_error("[Pager:seek_page]: Failed to seek");

  db_file.os_read(page_content, PAGE_SIZE);
  db_file.os_close();

  PagerBasePageHeader_t page_header(page_content);
  switch (page_header.page_type) {
  case PAGER_FIRST_PAGE: {
    FirstPageManager fpm(db_file_ptr_);
    page_manager_ = fpm;
    break;
  }
  case PAGER_NODE_PAGE: {
    NodePageManager npm(pgno, db_file_ptr_);
    page_manager_ = npm;
    break;
  }
  case PAGER_LEAF_PAGE: {
    LeafPageManager lpm(pgno, db_file_ptr_, this);
    page_manager_ = lpm;
    break;
  }
  case PAGER_FREE_PAGE: {
    FreePageManager fpm(pgno, db_file_ptr_);
    page_manager_ = fpm;
    break;
  }
  case PAGER_OVERFLOW_PAGE: {
    OverflowPageManager opm(pgno, db_file_ptr_);
    page_manager_ = opm;
    break;
  }
  default:
    throw std::runtime_error(
        "[Pager:seek_page]: Attempted to seek invalid page type");
  }
}

void Pager::insert_freelist(PageNumber pgno) {
  assert(db_file_ptr_ != nullptr);
  assert(pgno != 1);

  FirstPageManager fpm(db_file_ptr_);
  PageNumber curr_free_head = fpm.get_free_page_head();

  std::vector<std::byte> free_page_content(PAGE_SIZE);
  PagerFreePageHeader_t free_page_header(CHECKSUM, PAGER_FREE_PAGE,
                                         curr_free_head);
  std::memcpy(free_page_content.data(), free_page_header.to_bytes().data(),
              free_page_header.to_bytes().size());
  fpm.set_free_page_head(pgno);

  OsFile db_file = *db_file_ptr_;
  db_file.os_open();
  bool seek_ok = db_file.os_seek((pgno - 1) * PAGE_SIZE);
  if (!seek_ok)
    throw std::runtime_error("[Pager:insert_freelist]: Failed to seek");

  db_file.os_write(free_page_content, PAGE_SIZE);
  db_file.os_close();
}

PageNumber Pager::pop_freelist() {
  assert(db_file_ptr_ != nullptr);

  std::vector<std::byte> first_page_content(PAGE_SIZE);
  OsFile db_file = *db_file_ptr_;
  db_file.os_open();
  bool seek_ok = db_file.os_seek(0);
  if (!seek_ok)
    throw std::runtime_error("[Pager:pop_freelist]: Failed to seek first page");

  db_file.os_read(first_page_content, PAGE_SIZE);
  db_file.os_close();

  FirstPageManager fpm(db_file_ptr_);
  PageNumber free_head_pgno = fpm.get_free_page_head();
  if (free_head_pgno == NULL_PAGE)
    return NULL_PAGE;

  std::vector<std::byte> free_page_content(PAGE_SIZE);

  db_file.os_open();
  seek_ok = db_file.os_seek((free_head_pgno - 1) * PAGE_SIZE);
  if (!seek_ok)
    throw std::runtime_error(
        "[Pager:pop_freelist]: Failed to seek freelist head");

  db_file.os_read(free_page_content, PAGE_SIZE);
  db_file.os_close();

  PagerFreePageHeader_t free_page_header(free_page_content);
  fpm.set_free_page_head(free_page_header.next_free_page);

  return free_head_pgno;
}

PageNumber Pager::peek_freelist() {
  assert(db_file_ptr_ != nullptr);

  std::vector<std::byte> first_page_content(PAGE_SIZE);
  OsFile db_file = *db_file_ptr_;
  db_file.os_open();
  bool seek_ok = db_file.os_seek(0);
  if (!seek_ok)
    throw std::runtime_error("[Pager:peek_freelist]: Failed to seek");

  db_file.os_read(first_page_content, PAGE_SIZE);
  db_file.os_close();

  FirstPageManager fpm(db_file_ptr_);
  return fpm.get_free_page_head();
}

PageNumber Pager::create_page(PagerPageType page_type) {
  assert(page_type == PAGER_FREE_PAGE || page_type == PAGER_NODE_PAGE ||
         page_type == PAGER_LEAF_PAGE);
  assert(db_file_ptr_ != nullptr);

  switch (page_type) {
  case PAGER_FREE_PAGE: {
    FirstPageManager fp_manager(db_file_ptr_);

    total_num_pages_++;

    PageNumber new_free_pgno = total_num_pages_;
    PageNumber old_free_page_head = fp_manager.get_free_page_head();
    PagerFreePageHeader_t free_page_header =
        PagerFreePageHeader(CHECKSUM, PAGER_FREE_PAGE, old_free_page_head);

    OsFile db_file = *db_file_ptr_;
    db_file.os_open();
    db_file.os_append(free_page_header.to_bytes(), PAGE_SIZE);
    db_file.os_close();
    fp_manager.set_free_page_head(new_free_pgno);
    return new_free_pgno;
  }
  case PAGER_NODE_PAGE: {
    OsFile db_file = *db_file_ptr_;
    db_file.os_open();

    PageNumber new_pgno = total_num_pages_ + 1;
    PagerNodePageHeader_t node_page_header(
        CHECKSUM, PAGER_NODE_PAGE, 0, PAGE_SIZE - sizeof(PagerNodePageHeader_t),
        NULL_PAGE);

    db_file.os_append(node_page_header.to_bytes(), PAGE_SIZE);
    total_num_pages_ = new_pgno;
    db_file.os_close();

    FirstPageManager fpm(db_file_ptr_);
    fpm.set_num_pages(new_pgno);

    return new_pgno;
  }
  case PAGER_LEAF_PAGE: {
    OsFile db_file = *db_file_ptr_;
    db_file.os_open();

    PageNumber new_pgno = total_num_pages_ + 1;
    PagerLeafPageHeader_t leaf_page_header(
        CHECKSUM, PAGER_LEAF_PAGE, 0, PAGE_SIZE - sizeof(PagerLeafPageHeader_t),
        NULL_PAGE, NULL_PAGE);

    db_file.os_append(leaf_page_header.to_bytes(), PAGE_SIZE);
    total_num_pages_ = new_pgno;
    db_file.os_close();

    FirstPageManager fpm(db_file_ptr_);
    fpm.set_num_pages(new_pgno);
    return new_pgno;
  }
  default:
    throw std::runtime_error(
        "[Pager:create_page]: Can only create free or node pages");
  }
}

PageNumber Pager::_write_overflow_chain(std::vector<std::byte> payload) {
  assert(db_file_ptr_ != nullptr);

  const uint32_t chunk_size = PAGE_SIZE - sizeof(PagerOverflowPageHeader_t);
  const uint32_t payload_size = payload.size();

  std::vector<std::vector<std::byte>> chunks;
  std::vector<uint32_t> chunk_sizes;
  for (uint32_t offset = 0; offset < payload_size; offset += chunk_size) {
    uint32_t this_chunk_size = std::min(chunk_size, payload_size - offset);
    chunk_sizes.push_back(this_chunk_size);
    chunks.emplace_back(payload.begin() + offset,
                        payload.begin() + offset + this_chunk_size);
  }
  if (chunks.empty())
    return NULL_PAGE;

  FirstPageManager fpm(db_file_ptr_);
  PageNumber free_head = fpm.get_free_page_head();

  std::vector<PageNumber> page_nums;
  page_nums.reserve(chunks.size());

  OsFile db_file = *db_file_ptr_;

  for (size_t i = 0; i < chunks.size(); i++) {
    if (free_head != NULL_PAGE) {
      page_nums.push_back(free_head);
      FreePageManager tmp(free_head, db_file_ptr_);
      free_head = tmp.get_next_free_page();
      fpm.set_free_page_head(free_head);
    } else {
      total_num_pages_++;
      page_nums.push_back(total_num_pages_);

      std::vector<std::byte> blank(PAGE_SIZE, std::byte{0});
      db_file.os_open();
      db_file.os_append(blank, PAGE_SIZE);
      db_file.os_close();
    }
  }

  for (size_t i = 0; i < chunks.size(); i++) {
    PageNumber next_pgno =
        (i + 1 < chunks.size()) ? page_nums[i + 1] : NULL_PAGE;

    PagerOverflowPageHeader_t header(CHECKSUM, PAGER_OVERFLOW_PAGE, next_pgno,
                                     chunk_sizes[i]);
    std::vector<std::byte> page_data(PAGE_SIZE, std::byte{0});
    std::memcpy(page_data.data(), header.to_bytes().data(),
                sizeof(PagerOverflowPageHeader_t));
    std::memcpy(page_data.data() + sizeof(PagerOverflowPageHeader_t),
                chunks[i].data(), chunk_sizes[i]);

    db_file.os_open();
    bool seek_ok = db_file.os_seek((page_nums[i] - 1) * PAGE_SIZE);
    if (!seek_ok)
      throw std::runtime_error(
          "[Pager::_write_overflow_chain]: Failed to seek");
    bool write_ok = db_file.os_write(page_data, PAGE_SIZE);
    if (!write_ok)
      throw std::runtime_error(
          "[Pager::_write_overflow_chain]: Failed to write");
    db_file.os_close();
  }

  return page_nums[0];
}

PageNumber Pager::create_page(PagerPageType page_type,
                              std::vector<std::byte> payload) {
  assert(page_type == PAGER_OVERFLOW_PAGE);
  assert(db_file_ptr_ != nullptr);
  return _write_overflow_chain(payload);
}

void Pager::set_num_pages(PageNumber new_num_pages) {
  assert(db_file_ptr_ != nullptr);

  FirstPageManager fpm;
  fpm = std::get<FirstPageManager>(page_manager_);
  fpm.set_num_pages(new_num_pages);
}

PageNumber Pager::get_num_pages() {
  assert(db_file_ptr_ != nullptr);
  FirstPageManager fpm(db_file_ptr_);
  return fpm.num_pages_;
}

PagerPageType Pager::get_page_type(PageNumber pgno) {
  assert(db_file_ptr_ != nullptr);
  assert(pgno != NULL_PAGE);

  OsFile db_file = *db_file_ptr_;
  db_file.os_open();

  bool seek_ok = db_file.os_seek((pgno - 1) * PAGE_SIZE);
  if (!seek_ok)
    throw std::runtime_error("[Pager::get_page_type]: Failed to seek");

  std::vector<std::byte> page_contents(sizeof(PagerBasePageHeader_t));
  ssize_t bytes_read =
      db_file.os_read(page_contents, sizeof(PagerBasePageHeader_t));
  PagerBasePageHeader_t page_header(page_contents);
  db_file.os_close();

  if (page_header.checksum != CHECKSUM)
    throw std::runtime_error("[Pager:get_page_type]: Checksum failed");
  return page_header.page_type;
}
