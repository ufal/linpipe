// Based on https://www.man7.org/linux/man-pages/man2/mmap.2.html
#include <iostream>
#include <unistd.h>
// mmap for Linux
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "common.h"

#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

using namespace linpipe;

class MMAP{
  private:
    const char* addr;
    void* mmap_addr;
    int fd, length;
    void load(const char* path) {
      fd = open(path, O_RDONLY);
      struct stat sb;
      if (fd == -1)
        handle_error("open");
      if (fstat(fd, &sb) == -1)
        handle_error("stat");
      length = sb.st_size;

      mmap_addr = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0);
      if (mmap_addr == MAP_FAILED)
        handle_error("map");
      addr = static_cast<const char*>(mmap_addr);
    }
  public:
    MMAP(const char* path) {
      load(path);
    }
    void read(int start, int end) {
      for (int i = start; i < min(end, length); ++i) {
        printf("%c\n", addr[i]);
      }
    }
    void read_int_like(int start, int end) {
      for (int i = start; (i + 4) <= min(end, length); i += 4) {
        int x = 0;
        for (int j = 3; j >= 0; --j) {
          x = x | (((int)addr[i + j]) << (j * 8));
        }
        printf("%d\n", x);
      }
    }
    ~MMAP() {
      munmap(mmap_addr, length);
      close(fd);
    }
};

