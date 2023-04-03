// Based on https://www.man7.org/linux/man-pages/man2/mmap.2.html
#include <filesystem>
#include <iostream>
#ifdef _WIN32
  #include <Windows.h>
  #include <memoryapi.h>
  #include <winbase.h>
  #include <fileapi.h>
#else
  #include <unistd.h>
  #include <sys/mman.h>
  #include <sys/stat.h>
  #include <fcntl.h>
#endif

#include "common.h"

#define handle_error(msg) do { perror(msg); exit(EXIT_FAILURE); } while (0)

using namespace linpipe;

class MMAP{
  private:
    void* mmap_addr;
    size_t length;
#ifdef _WIN32
    HANDLE fd;
#else
    int fd;
#endif
    void load(filesystem::path path) {
#ifdef _WIN32
      HANDLE temp = CreateFile(path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
      length = GetFileSize(temp, NULL);
      fd = CreateFileMapping(temp, NULL, PAGE_READONLY, 0, 0, NULL);
      mmap_addr = MapViewOfFile(fd, FILE_MAP_ALL_ACCESS, 0, 0, 0);
      CloseHandle(temp);
#else
      fd = open(path.c_str(), O_RDONLY);
      struct stat sb;
      if (fd == -1)
        handle_error("open");
      if (fstat(fd, &sb) == -1)
        handle_error("stat");
      length = sb.st_size;

      mmap_addr = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, 0);
      if (mmap_addr == MAP_FAILED)
        handle_error("map");
#endif
    }
  public:
    MMAP(filesystem::path path) {
      load(path);
    }
    void read(size_t start, size_t end) {
      for (size_t i = start; i < min(end, length); ++i) {
        printf("%c\n", ((char*)mmap_addr)[i]);
      }
    }
    /*
    void read_int_like(int start, int end) {
      for (int i = start; (i + 4) <= min(end, length); i += 4) {
        int x = 0;
        for (int j = 3; j >= 0; --j) {
          x = x | (((int)addr[i + j]) << (j * 8));
        }
        printf("%d\n", x);
      }
    }
    */
    ~MMAP() {
#ifdef _WIN32
      UnmapViewOfFile(mmap_addr);
      CloseHandle(fd);
#else
      munmap(mmap_addr, length);
      close(fd);
#endif
    }
};

