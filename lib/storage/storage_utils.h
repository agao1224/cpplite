#pragma once

#include "shared.h"
#include <cstdint>
#include <string>

namespace storage {

using SegmentID = uint64_t;

inline std::string tbl_path(TableID tbl_id) {
  return std::to_string(tbl_id);
}

inline std::string seg_path(TableID tbl_id, SegmentID seg_id) {
  return std::to_string(tbl_id) + "/" + std::to_string(seg_id) + ".db";
}

inline SegmentID pgno_to_segid(PageNumber pgno, uint64_t pages_per_segment) {
  return static_cast<uint64_t>(pgno) / pages_per_segment;
}

inline PageNumber pgno_to_rel_pgno(PageNumber pgno, uint64_t pages_per_segment) {
  return static_cast<uint64_t>(pgno) % pages_per_segment;
}

inline uint64_t pgno_to_offset(PageNumber pgno, uint64_t page_size) {
  return static_cast<uint64_t>(pgno) * page_size;
}

inline TableID path_to_tbl_id(const std::string &path) {
  size_t last_slash = path.rfind('/');
  std::string dirname = (last_slash == std::string::npos) ? path : path.substr(last_slash + 1);
  return static_cast<TableID>(std::stoull(dirname));
}

} // namespace storage
