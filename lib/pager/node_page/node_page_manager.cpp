#include <cstddef>
#include <cstring>
#include <cstddef>
#include <cassert>
#include <optional>
#include <stdexcept>

#include "node_page.h"
#include "pager/base_page.h"


NodePageManager::NodePageManager(PageNumber pgno, std::shared_ptr<OsFile> db_file_ptr) {
  assert(db_file_ptr != nullptr);

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
  checksum_ = node_page_header.checksum;

  if (checksum_ == CHECKSUM) {
    assert(node_page_header.page_type == PAGER_NODE_PAGE);
    page_type_ = node_page_header.page_type;
    num_cells_ = node_page_header.num_cells;
    total_bytes_free_ = node_page_header.total_bytes_free;
    right_child_ = node_page_header.right_child;
    pgno_ = pgno;

    cells_.resize(num_cells_);
    std::memcpy(
      cells_.data(),
      page_content.data() + sizeof(PagerNodePageHeader_t),
      num_cells_ * sizeof(NodeCell_t)
    );
  } else {
    throw std::runtime_error("[NodePageManager]: Invalid checksum");
  }
}

NodePageManager::~NodePageManager() = default;

bool NodePageManager::insert_node_cell(uint32_t key) {
  assert(num_cells_ == cells_.size());
  assert(db_file_ptr_ != nullptr);

  // NOTE(andrew): Not enough space to write in node cell,
  // the caller should handle creating another node page to
  // with a new cell in it
  if (sizeof(NodeCell_t) > total_bytes_free_)
    return false;

  assert(sizeof(NodeCell_t) <= total_bytes_free_);
  NodeCell_t node_cell(key, NULL_PAGE);

  uint16_t free_bytes = total_bytes_free_ - sizeof(NodeCell_t);

  cells_.push_back(node_cell);
  std::vector<std::byte> cell_bytes(cells_.size() * sizeof(NodeCell_t));
  std::memcpy(cell_bytes.data(), cells_.data(), cell_bytes.size());

  OsFile db_file = *db_file_ptr_;
  bool seek_ok, write_ok;

  db_file.os_open();
  seek_ok = db_file.os_seek((pgno_-1)*PAGE_SIZE + sizeof(PagerNodePageHeader_t));
  if (!seek_ok)
    throw std::runtime_error("[NodePageManager:insert_node_cell]: Failed to seek to write cell header");
  write_ok = db_file.os_write(cell_bytes, cell_bytes.size());
  if (!write_ok)
    throw std::runtime_error("[NodePageManager:insert_node_cell]: Failed to write cell header");

  total_bytes_free_ -= sizeof(NodeCell_t);
  num_cells_++;
  PagerNodePageHeader_t node_page_header(
    CHECKSUM,
    PAGER_NODE_PAGE,
    num_cells_,
    total_bytes_free_,
    right_child_
  );

  size_t expected_bytes = (
    sizeof(PagerNodePageHeader_t)
    + (num_cells_ * sizeof(NodeCell_t))
    + total_bytes_free_
  );
  assert(expected_bytes == PAGE_SIZE);

  seek_ok = db_file.os_seek((pgno_-1)*PAGE_SIZE);
  if (!seek_ok)
    throw std::runtime_error("[NodePageManager:insert_node_cell]: Failed to seek to write new node page header");
  write_ok = db_file.os_write(node_page_header.to_bytes(), sizeof(PagerNodePageHeader_t));
  if (!write_ok)
    throw std::runtime_error("[NodePageManager:insert_node_cell]: Failed to write new node page header");
  db_file.os_close();

  return true;
}

std::optional<size_t> NodePageManager::find_node_cell_idx(uint32_t target_key) {
  assert(cells_.size() == num_cells_);
  size_t lo = 0;
  size_t hi = num_cells_;
  while (lo < hi) {
    size_t mid = lo + (hi - lo)/2;
    uint32_t node_key = cells_[mid].key;

    if (target_key < node_key) hi = mid;
    else if (node_key < target_key) lo = mid+1;
    else return mid;
  }
  return std::nullopt;
}

void NodePageManager::set_cell_left_child(uint32_t key, PageNumber left_child_pgno) {
  assert(db_file_ptr_ != nullptr);
  assert(num_cells_ == cells_.size());
  std::optional<size_t> key_idx_opt = find_node_cell_idx(key);
  if (!key_idx_opt.has_value())
    throw std::runtime_error("[NodePageManager:set_cell_left_child]: Key not found");

  size_t key_idx = key_idx_opt.value();
  assert(cells_[key_idx].key == key);
  cells_[key_idx].left_child = left_child_pgno;
  NodeCell_t updated_node_cell(key, left_child_pgno);

  OsFile db_file = *db_file_ptr_;
  db_file.os_open();

  // NOTE(andrew): This approach of finding the exact offset of
  // the NodeCell_t and overwriting sizeof(NodeCell_t) will not work
  // when switchin to slotted pages. Need to make sure to change in the future.
  off_t updated_cell_offset = (
    (pgno_-1)*PAGE_SIZE + sizeof(PagerNodePageHeader_t) + key_idx*(sizeof(NodeCell_t))
  );
  bool seek_ok = db_file.os_seek(updated_cell_offset);
  if (!seek_ok) throw std::runtime_error("[NodePageManager:set_cell_left_child]: Failed to seek");
  
  bool write_ok = db_file.os_write(updated_node_cell.to_bytes(), sizeof(NodeCell_t));
  if (!write_ok)
    throw std::runtime_error("[NodePageManager:set_cell_left_child]: Failed to write updated cell");
  db_file.os_close();
  return;
}

void NodePageManager::set_node_right_child(PageNumber new_right_child) {
  assert(db_file_ptr_ != nullptr);

  PagerNodePageHeader_t updated_node_page_header(
    CHECKSUM,
    PAGER_NODE_PAGE,
    num_cells_,
    total_bytes_free_,
    new_right_child
  );
  right_child_ = new_right_child;

  OsFile db_file = *db_file_ptr_;
  db_file.os_open();
  bool seek_ok = db_file.os_seek((pgno_-1)*PAGE_SIZE);
  if (!seek_ok) throw std::runtime_error("[NodePageManager:set_node_right_child]: Failed to seek");

  bool write_ok = db_file.os_write(updated_node_page_header.to_bytes(), sizeof(PagerNodePageHeader_t));
  if (!write_ok) throw std::runtime_error("[NodePageManager:set_node_right_child]: Failed to write");
  db_file.os_close();
  return;
}

size_t NodePageManager::get_free_space() {
  return total_bytes_free_;
}
