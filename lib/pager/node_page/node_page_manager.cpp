#include <cstddef>
#include <cstring>
#include <cstddef>
#include <cassert>
#include <iostream>
#include <stdexcept>

#include "node_page.h"
#include "pager/pager.h"
#include "pager/overflow_page/overflow_page.h"


NodePageManager::NodePageManager(PageNumber pgno, std::shared_ptr<OsFile> db_file_ptr, Pager* pager) {
  assert(db_file_ptr != nullptr);
  assert(pager != nullptr);

  std::vector<std::byte> page_content(PAGE_SIZE);

  OsFile db_file = *db_file_ptr;
  db_file.os_open();

  // NOTE(andrew): consider creating a universal helper function that finds the exact
  // offset of specific properties like pages, or even fields within pages
  // since this single operation is being done a lot
  bool seek_ok = db_file.os_seek((pgno-1)*PAGE_SIZE);
  if (!seek_ok)
    throw std::runtime_error("[NodePageManager]: Failed to seek");

  ssize_t bytes_read = db_file.os_read(page_content, PAGE_SIZE);
  db_file.os_close();

  PagerNodePageHeader_t node_page_header(page_content);

  db_file_ptr_ = db_file_ptr;
  pager_ = pager;
  checksum_ = node_page_header.checksum;
  int32_t num_payload_bytes = (static_cast<int32_t>(bytes_read)
    - static_cast<int32_t>(sizeof(node_page_header)));

  if (checksum_ == CHECKSUM && num_payload_bytes >= 0) {
    assert(node_page_header.page_type == PAGER_NODE_PAGE);
    page_type_ = node_page_header.page_type;
    num_cells_ = node_page_header.num_cells;
    total_bytes_free_ = node_page_header.total_bytes_free;
    pgno_ = pgno;

    data_.resize(num_payload_bytes);

    size_t num_cell_bytes = static_cast<size_t>(bytes_read - sizeof(PagerNodePageHeader_t));

    cells_.resize(num_cells_);
    std::memcpy(
      cells_.data(),
      page_content.data() + sizeof(PagerNodePageHeader_t),
      num_cells_ * sizeof(PagerCell_t)
    );

    std::memcpy(data_.data(),
                page_content.data() + sizeof(PagerNodePageHeader_t),
                static_cast<size_t>(PAGE_SIZE - sizeof(PagerNodePageHeader_t)));
  } else {
    throw std::runtime_error("[NodePageManager]: Invalid checksum");
  }
}

NodePageManager::~NodePageManager() = default;

bool NodePageManager::insert_cell(uint32_t key, std::vector<std::byte> cell_data) {
  assert(num_cells_ == cells_.size());
  assert(db_file_ptr_ != nullptr);
  assert(pager_ != nullptr);

  // NOTE(andrew): Not enough space to write in pager cell,
  // the caller should handle creating another node page to
  // fit this in
  if (sizeof(PagerCell_t) > total_bytes_free_)
    return false;

  assert(sizeof(PagerCell_t) <= total_bytes_free_);
  PagerCell_t pager_cell;
  uint32_t payload_size = cell_data.size();
  pager_cell.size = payload_size;
  pager_cell.key = key;
  pager_cell.left_child = NULL_PAGE;
  pager_cell.record_page = pager_->create_page(PAGER_OVERFLOW_PAGE, cell_data);

  uint16_t free_bytes = total_bytes_free_ - sizeof(PagerCell_t);

  cells_.push_back(pager_cell);
  std::vector<std::byte> cell_bytes(cells_.size() * sizeof(PagerCell_t));
  std::memcpy(cell_bytes.data(), cells_.data(), cell_bytes.size());

  OsFile db_file = *db_file_ptr_;
  bool seek_ok, write_ok;

  db_file.os_open();
  seek_ok = db_file.os_seek((pgno_-1)*PAGE_SIZE + sizeof(PagerNodePageHeader_t));
  if (!seek_ok)
    throw std::runtime_error("[NodePageManager:insert_cell]: Failed to seek to write cell header");
  write_ok = db_file.os_write(cell_bytes, cell_bytes.size());
  if (!write_ok)
    throw std::runtime_error("[NodePageManager:insert_cell]: Failed to write cell header");

  total_bytes_free_ -= sizeof(PagerCell_t);
  num_cells_++;
  PagerNodePageHeader_t node_page_header(
    CHECKSUM,
    PAGER_NODE_PAGE,
    num_cells_,
    total_bytes_free_
  );

  size_t expected_bytes = (
    sizeof(PagerNodePageHeader_t)
    + (num_cells_ * sizeof(PagerCell_t))
    + total_bytes_free_
  );
  assert(expected_bytes == PAGE_SIZE);

  seek_ok = db_file.os_seek((pgno_-1)*PAGE_SIZE);
  if (!seek_ok)
    throw std::runtime_error("[NodePageManager:insert_cell]: Failed to seek to write new node page header");
  write_ok = db_file.os_write(node_page_header.to_bytes(), sizeof(PagerNodePageHeader_t));
  if (!write_ok)
    throw std::runtime_error("[NodePageManager:insert_cell]: Failed to write new node page header");
  db_file.os_close();

  return true;
}

std::optional<std::vector<std::byte>> NodePageManager::get_payload(uint32_t key) {
  assert(db_file_ptr_ != nullptr);

  std::optional<PagerCell_t> cell = std::nullopt;
  for (size_t i = 0; i < cells_.size(); i++) {
    if (cells_[i].key == key) {
      cell = cells_[i];
      break;
    }
  }

  if (!cell.has_value())
    return std::nullopt;

  uint32_t num_payload_bytes = cell->size;
  std::vector<std::byte> payload;
  payload.reserve(num_payload_bytes);

  PageNumber it = cell->record_page;
  while (it != NULL_PAGE) {
    OverflowPageManager page_manager(it, db_file_ptr_);
    std::vector<std::byte> overflow_content = page_manager.get_overflow_content();
    payload.insert(payload.end(), overflow_content.begin(), overflow_content.end());
    it = page_manager.get_next_overflow_page();
  }

  assert(payload.size() == num_payload_bytes);
  return payload;
}

size_t NodePageManager::get_free_space() {
  return total_bytes_free_;
}
