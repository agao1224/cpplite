#include <string>
#include "../shared.h"

class OsFile {
  public:
    OsFile(const std::string& path);
    ~OsFile();

    bool read_page(PageNumber page_number, std::array<std::byte, PAGE_SIZE> page_content);
    bool write_page(PageNumber page_number, std::array<std::byte, PAGE_SIZE> bytes_to_write, size_t num_bytes);
    void flush();

  private:
    int fd_;
};
