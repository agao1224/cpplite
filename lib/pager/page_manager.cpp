#include <cstddef>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <cstddef>
#include <cassert>

#include "base_page.h"
#include "pager/first_page/first_page.h"
#include "pager/node_page/node_page.h"

void BasePageManager::set_data(std::vector<std::byte> buffer, size_t num_bytes, PageNumber page_num) {
  assert(db_file_ptr_ != nullptr);

  OsFile db_file = *db_file_ptr_;
  db_file.os_open();
  size_t data_offset;

  if (page_num == 1) {
    assert(page_type_ == PAGER_FIRST_PAGE);
    data_offset = sizeof(PagerFirstPageHeader_t);
  } else {
    assert(page_type_ != PAGER_FIRST_PAGE);
    data_offset = (page_num-1) * PAGE_SIZE + sizeof(PagerNodePageHeader_t);
  }

  char errmsg[255];
  const char* page_manager = (page_type_ == PAGER_FIRST_PAGE) ? "FirstPageManager" : "NodePageManager";

  bool seek_ok = db_file.os_seek(static_cast<off_t>(data_offset));
  if (!seek_ok) {
    snprintf(errmsg, 255, "[%s]: Failed to seek data", page_manager);
    throw std::runtime_error(errmsg);
  }
  bool write_ok = db_file.os_write(buffer, num_bytes);
  if (!write_ok) {
    snprintf(errmsg, 255, "[%s]: Failed to write data", page_manager);
    throw std::runtime_error(errmsg);
  }
}
