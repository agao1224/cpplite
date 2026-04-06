#include <cstddef>
#include <cstring>
#include <cstddef>
#include <cassert>
#include <stdexcept>

#include "leaf_page.h"
#include "pager/base_page.h"
#include "pager/pager.h"
#include "pager/overflow_page/overflow_page.h"
#include "shared.h"


LeafPageManager::LeafPageManager(PageNumber pgno, std::shared_ptr<OsFile> db_file_ptr, Pager* pager) {
  assert(db_file_ptr != nullptr);
  assert(pager != nullptr);

  std::vector<std::byte> page_content(PAGE_SIZE);

  OsFile db_file = *db_file_ptr;
  db_file.os_open();

  bool seek_ok = db_file.os_seek((pgno-1)*PAGE_SIZE);
  if (!seek_ok)
    throw std::runtime_error("[LeafPageManager]: Failed to seek");

  ssize_t bytes_read = db_file.os_read(page_content, PAGE_SIZE);
  db_file.os_close();

  PagerLeafPageHeader_t leaf_page_header(page_content);

  db_file_ptr_ = db_file_ptr;
  pager_ = pager;
  checksum_ = leaf_page_header.checksum;
  int32_t num_payload_bytes = (static_cast<int32_t>(bytes_read)
    - static_cast<int32_t>(sizeof(leaf_page_header)));

  if (checksum_ == CHECKSUM && num_payload_bytes >= 0) {
    assert(leaf_page_header.page_type == PAGER_LEAF_PAGE);
    page_type_ = leaf_page_header.page_type;
    num_cells_ = leaf_page_header.num_cells;
    total_bytes_free_ = leaf_page_header.total_bytes_free;
    prev_page_ = leaf_page_header.prev_page;
    next_page_ = leaf_page_header.next_page; 
    pgno_ = pgno;

    data_.resize(num_payload_bytes);

    size_t num_cell_bytes = static_cast<size_t>(bytes_read - sizeof(PagerLeafPageHeader_t));

    cells_.resize(num_cells_);
    std::memcpy(
      cells_.data(),
      page_content.data() + sizeof(PagerLeafPageHeader_t),
      num_cells_ * sizeof(LeafCell_t)
    );

    std::memcpy(data_.data(),
                page_content.data() + sizeof(PagerLeafPageHeader_t),
                static_cast<size_t>(PAGE_SIZE - sizeof(PagerLeafPageHeader_t)));
  } else {
    throw std::runtime_error("[LeafPageHeader]: Invalid checksum");
  }
}

LeafPageManager::~LeafPageManager() = default;

bool LeafPageManager::insert_cell(DefaultPagerKey key, std::vector<std::byte> cell_data) {
  assert(num_cells_ == cells_.size());
  assert(db_file_ptr_ != nullptr);
  assert(pager_ != nullptr);

  if (sizeof(LeafCell_t) > total_bytes_free_)
    return false;

  assert(sizeof(LeafCell_t) <= total_bytes_free_);
  size_t lo = 0, hi = cells_.size();
  while (lo < hi) {
    size_t mid = lo + (hi - lo) / 2;
    if (key < cells_[mid].key) hi = mid;
    else if (cells_[mid].key < key) lo = mid + 1;
    else throw std::runtime_error("[LeafPageManager]: Duplicate leaf cell key");
  }

  LeafCell_t leaf_cell;
  leaf_cell.payload_size = cell_data.size();
  leaf_cell.key = key;
  leaf_cell.record_page = pager_->create_page(PAGER_OVERFLOW_PAGE, cell_data);

  cells_.insert(cells_.begin() + lo, leaf_cell);
  std::vector<std::byte> cell_bytes(cells_.size() * sizeof(LeafCell_t));
  std::memcpy(cell_bytes.data(), cells_.data(), cell_bytes.size());

  OsFile db_file = *db_file_ptr_;
  bool seek_ok, write_ok;

  db_file.os_open();
  seek_ok = db_file.os_seek((pgno_-1)*PAGE_SIZE + sizeof(PagerLeafPageHeader_t));
  if (!seek_ok)
    throw std::runtime_error("[LeafPageManager:insert_cell]: Failed to seek write cell header");
  write_ok = db_file.os_write(cell_bytes, cell_bytes.size());
  if (!write_ok)
    throw std::runtime_error("[LeafPageManager:insert_cell]: Failed to write cell header");

  total_bytes_free_ -= sizeof(LeafCell_t);
  num_cells_++;
  PagerLeafPageHeader_t leaf_page_header(
    CHECKSUM,
    PAGER_LEAF_PAGE,
    num_cells_,
    total_bytes_free_,
    next_page_,
    prev_page_
  );

  size_t expected_bytes = (
    sizeof(PagerLeafPageHeader_t)
    + (num_cells_ * sizeof(LeafCell_t))
    + total_bytes_free_
  );
  assert(expected_bytes == PAGE_SIZE);

  seek_ok = db_file.os_seek((pgno_-1)*PAGE_SIZE);
  if (!seek_ok)
    throw std::runtime_error("[LeafPageManager:insert_cell]: Failed to seek to write new leaf page header");
  write_ok = db_file.os_write(leaf_page_header.to_bytes(), sizeof(PagerLeafPageHeader_t));
  if (!write_ok)
    throw std::runtime_error("[LeafPageManager:insert_cell]: Failed to write new leaf page header");
  db_file.os_close();

  return true;
}

bool LeafPageManager::write_cell(LeafCell_t cell) {
  assert(num_cells_ == cells_.size());
  assert(db_file_ptr_ != nullptr);
  assert(pager_ != nullptr);

  if (sizeof(LeafCell_t) > total_bytes_free_)
    return false;

  size_t lo = 0, hi = cells_.size();
  while (lo < hi) {
    size_t mid = lo + (hi - lo) / 2;
    if (cell.key < cells_[mid].key) hi = mid;
    else if (cells_[mid].key < cell.key) lo = mid + 1;
    else throw std::runtime_error("[LeafPageManager]: Duplicate leaf cell key");
  }

  cells_.insert(cells_.begin() + lo, cell);
  std::vector<std::byte> cell_bytes(cells_.size() * sizeof(LeafCell_t));
  std::memcpy(cell_bytes.data(), cells_.data(), cell_bytes.size());

  total_bytes_free_ -= sizeof(LeafCell_t);
  num_cells_++;

  PagerLeafPageHeader_t leaf_page_header(
    CHECKSUM,
    PAGER_LEAF_PAGE,
    num_cells_,
    total_bytes_free_,
    next_page_,
    prev_page_
  );

  size_t expected_bytes = (
    sizeof(PagerLeafPageHeader_t)
    + (num_cells_ * sizeof(LeafCell_t))
    + total_bytes_free_
  );
  assert(expected_bytes == PAGE_SIZE);

  OsFile db_file = *db_file_ptr_;
  db_file.os_open();

  bool seek_ok = db_file.os_seek((pgno_-1)*PAGE_SIZE + sizeof(PagerLeafPageHeader_t));
  if (!seek_ok)
    throw std::runtime_error("[LeafPageManager:write_cell]: Failed to seek to write cells");
  bool write_ok = db_file.os_write(cell_bytes, cell_bytes.size());
  if (!write_ok)
    throw std::runtime_error("[LeafPageManager:write_cell]: Failed to write cells");

  seek_ok = db_file.os_seek((pgno_-1)*PAGE_SIZE);
  if (!seek_ok)
    throw std::runtime_error("[LeafPageManager:write_cell]: Failed to seek to write header");
  write_ok = db_file.os_write(leaf_page_header.to_bytes(), sizeof(PagerLeafPageHeader_t));
  if (!write_ok)
    throw std::runtime_error("[LeafPageManager:write_cell]: Failed to write header");

  db_file.os_close();
  return true;
}

std::optional<std::vector<std::byte>> LeafPageManager::get_payload(DefaultPagerKey key) {
  assert(db_file_ptr_ != nullptr);

  std::optional<LeafCell_t> cell = std::nullopt;
  for (size_t i = 0; i < cells_.size(); i++) {
    if (cells_[i].key == key) {
      cell = cells_[i];
      break;
    }
  }

  if (!cell.has_value())
    return std::nullopt;

  uint32_t num_payload_bytes = cell->payload_size;
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

size_t LeafPageManager::get_free_space() {
  return total_bytes_free_;
}

PageNumber LeafPageManager::prev_page() {
  return prev_page_;
}

PageNumber LeafPageManager::next_page() {
  return next_page_;
}

void LeafPageManager::_set_leaf_page_pointers(PageNumber pgno, bool use_prev_page) {
  assert(db_file_ptr_ != nullptr);

  OsFile db_file = *db_file_ptr_;
  db_file.os_open();
  bool seek_ok = db_file.os_seek((pgno_-1)*PAGE_SIZE);
  if (!seek_ok)
    throw std::runtime_error("[LeafPageManager:_set_leaf_page_pointers]: Failed to seek");

  std::vector<std::byte> leaf_page_content(sizeof(PagerLeafPageHeader_t));
  ssize_t bytes_read = db_file.os_read(leaf_page_content, sizeof(PagerLeafPageHeader_t));

  PagerLeafPageHeader_t leaf_page_header(leaf_page_content);
  if (use_prev_page) {
    leaf_page_header.prev_page = pgno;
    prev_page_ = pgno;
  } else {
    leaf_page_header.next_page = pgno;
    next_page_ = pgno;
  }

  leaf_page_content = leaf_page_header.to_bytes();
  seek_ok = db_file.os_seek((pgno_-1)*PAGE_SIZE);
  if (!seek_ok)
    throw std::runtime_error("[LeafPageManager:_set_leaf_page_pointers]: Failed to seek before write");
  bool write_ok = db_file.os_write(leaf_page_content, sizeof(PagerLeafPageHeader_t));
  if (!write_ok)
    throw std::runtime_error("[LeafPageManager:_set_leaf_page_pointers]: Failed to write new header");
  db_file.os_close();
  return;
}

void LeafPageManager::set_prev_page(PageNumber pgno) {
  _set_leaf_page_pointers(pgno, true);
}

void LeafPageManager::set_next_page(PageNumber pgno) {
  _set_leaf_page_pointers(pgno, false);
}