#pragma once

#include "shared.h"
#include <cstdint>
#include <string>

static const uint32_t DEFAULT_SEGMENT_SIZE = 131072;
static const uint32_t DEFAULT_PAGE_SIZE = 4096;
static const std::string CONTROL_FILE = "db.control";

namespace storage {

struct EngineConfig {
  uint64_t segment_size = DEFAULT_SEGMENT_SIZE;
  uint64_t page_size = DEFAULT_PAGE_SIZE;
};

struct ControlRecord {
  PageKey catalog_key;
};

} // namespace storage

class StorageEngine {
private:
  std::string basepath_;
  storage::EngineConfig config_;
  storage::ControlRecord control_;

  void init_control();

public:
  StorageEngine(const std::string &basepath);
};
