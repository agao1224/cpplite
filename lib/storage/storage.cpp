

#include "storage/storage.h"

void StorageEngine::init_control() {}
StorageEngine::StorageEngine(const std::string &basepath) {
  basepath_ = basepath;
}
