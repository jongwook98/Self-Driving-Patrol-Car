/* Copyright 2022, Kim, Jinseong all rights reserved */

#include <lane_detection/common/common.h>
#include <lane_detection/common/shared_memory.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <cstring>

static inline off_t _align_offset(const off_t mem_size);
static inline int _memory_copy(void *dest, const void *src, const off_t offset);

SharedMemory::SharedMemory(const char *shm_path, const off_t mem_size)
    : fd(-EPERM), req_size(0), virt_addr(nullptr), shm_path(shm_path) {
  int ret = Open(shm_path, mem_size);
  EXIT_GUARD(ret < 0, "[SM] in Constructor");
}

SharedMemory::~SharedMemory() { Close(); }

int SharedMemory::CopyToMem(const void *write_buf, off_t offset) {
  return _memory_copy(virt_addr, write_buf, offset);
}

int SharedMemory::CopyFromMem(void *read_buf, off_t offset) {
  return _memory_copy(read_buf, virt_addr, offset);
}

int SharedMemory::Open(const char *shm_path, const off_t mem_size) {
  fd = shm_open(shm_path, O_RDWR | O_CREAT | O_EXCL, 0666);
  FORMULA_GUARD(fd == -EPERM, -EPERM, "Failed to open the shared memory.");

  req_size = _align_offset(mem_size);
  int ret = ftruncate(fd, req_size);
  FORMULA_GUARD_WITH_EXIT_FUNC(ret == -EPERM, -EPERM, Close(),
                               "Failed to truncate the file.");

  virt_addr = mmap(NULL, req_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  FORMULA_GUARD_WITH_EXIT_FUNC(virt_addr == MAP_FAILED, -EPERM, Close(),
                               "Failed to mapping the memory.");

  return 0;
}

int SharedMemory::Close(void) {
  if (virt_addr && req_size)
    munmap(virt_addr, req_size);

  if (fd > 0) {
    close(fd);
    shm_unlink(shm_path);
  }

  fd = -EPERM;
  req_size = 0;

  virt_addr = nullptr;
  shm_path = nullptr;

  return 0;
}

static inline off_t _align_offset(const off_t mem_size) {
  off_t page_size = static_cast<off_t>(sysconf(_SC_PAGESIZE));
  return (mem_size + page_size) & ~(page_size - 1);
}

static inline int _memory_copy(void *dest, const void *src,
                               const off_t offset) {
  FORMULA_GUARD(src == nullptr, -EPERM, ERR_INVALID_PTR);

  std::size_t n = static_cast<std::size_t>(offset);
  memmove(dest, src, n);
  msync(dest, n, MS_SYNC | MS_INVALIDATE);
  return static_cast<int>(n);
}
