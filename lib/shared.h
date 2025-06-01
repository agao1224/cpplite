#include <cstdint>
#include <cstddef>

#ifndef SHARED_H
#define SHARED_H

typedef uint64_t PageNumber;
const size_t PAGE_SIZE = 4096;
extern const char* DB_FILENAME;
const uint32_t CHECKSUM = 123456; 

#endif