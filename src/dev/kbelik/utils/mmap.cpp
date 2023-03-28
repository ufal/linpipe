// Based on https://www.man7.org/linux/man-pages/man2/mmap.2.html
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
    const char* addr;
    void* mmap_addr;
#ifdef _WIN32
    HANDLE fd;
    LARGE_INTEGER length;
#else
    int length;
    int fd;
#endif
    void load(const char* path) {
#ifdef _WIN32
      HANDLE temp = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
      GetFileSizeEx(temp, &length);
      CloseHandle(temp);
      fd = CreateFileMapping(HANDLE, NULL, PAGE_READ, 
          length.HighPart, length.LowPart, NULL);
      mmaP_addr = MapViewOfFile(fd, FILE_MAP_ALL_ACCESS, 0, 0, length);
#else
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
#endif
      addr = static_cast<const char*>(mmap_addr);
    }
  public:
    MMAP(const char* path) {
      load(path);
    }
    /*
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
    */
    ~MMAP() {
#ifdef _WIN32
#else
      munmap(mmap_addr, length);
      close(fd);
#endif
    }
};

