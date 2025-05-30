#include "osfile.h"
#include "../shared.h"
#include <array>
#include <cstddef>
#include <sys/fcntl.h>
#include <stdio.h>
#include <unistd.h>


OsFile::OsFile(const std::string& path) {
  fd_ = open(path.c_str(), O_RDWR | O_CREAT, 0644);
  if (fd_ == -1)
    perror("[VFS/New]: Failed to open DB file");
}

OsFile::~OsFile() {
  if (fd_ != -1)
    close(fd_);
}

/**
 * @brief 
 * 
 * @param page_number Page number offset to read from
 * @param page_content Array of bytes to write data into
 * @return true: if reading from DB file was successful
 * @return false: if reading from DB file failed
 */
bool OsFile::read_page(PageNumber page_number, std::array<std::byte, PAGE_SIZE> page_content) {
  if (fd_ == -1) {
    perror("[VFS/Read]: Unable to open DB file");
    return false;
  }

  off_t file_offset = PAGE_SIZE * page_number;
  ssize_t bytes_read = pread(fd_, page_content.data(), PAGE_SIZE, file_offset);
  if (bytes_read == -1) {
    perror("[VFS/Read]: Error reading bytes from DB file");
    return false;
  }
  return true;
}

/**
 * @brief 
 * 
 * @param page_number Page number offset to write at disk
 * @param bytes_to_write Byte content to write to file
 * @param num_bytes Number of bytes to write
 * @return true: if writing to the DB file was successful
 * @return false: if failed writing to the DB file
 */
bool OsFile::write_page(
  PageNumber page_number,
  std::array<std::byte, PAGE_SIZE> bytes_to_write,
  size_t num_bytes
) {
  if (fd_ == -1) {
    perror("[VFS/Write]: Unable to open DB file");
    return false;
  }

  off_t file_offset = PAGE_SIZE * page_number;
  ssize_t bytes_written = pwrite(fd_, bytes_to_write.data(), num_bytes, file_offset);
  if (bytes_written != num_bytes) {
    fprintf(stderr, "[VFS/Write]: Failed to write %zu bytes (wrote %zd)\n", num_bytes, bytes_written);
    return false;
  }
  return true;
}