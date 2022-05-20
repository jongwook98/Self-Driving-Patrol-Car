/* Copyright 2022, Kim, Jinseong all rights reserved */

#include <lane_detection/common/shared_memory.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstring>

SharedMemory::SharedMemory(const char *shm_path, const std::size_t mem_size)
    : fd(static_cast<int>(-1)), mem_size(mem_size), shm_path(shm_path) {
  int ret = Open(shm_path, mem_size);
  if (ret < 0)
    exit(-EPERM);
}

SharedMemory::~SharedMemory() { Close(); }

void SharedMemory::ShmCopy(void *shm_path, const void *input_path) {
  memcpy(shm_path, input_path, mem_size);
}

int SharedMemory::Open(const char *shm_path, const std::size_t mem_size) {
  fd = shm_open(shm_path, O_RDWR | O_CREAT, 0666);
  if (fd == static_cast<int>(-1)) {
    std::cout << "Failed to open the shared memory." << std::endl;
    return -EPERM;
  }

  int ret = ftruncate(fd, mem_size);
  if (ret == -EPERM) {
    std::cout << "Failed to trucate the file." << std::endl;
    return -EPERM;
  }

  shm_addr = mmap(NULL, mem_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (shm_addr == MAP_FAILED) {
    std::cout << "Failed to map the memory." << std::endl;
    return -EPERM;
  }

  return 0;
}

int SharedMemory::Close() {
  if (fd > 0)
    close(fd);

  fd = static_cast<int>(-1);
  mem_size = 0;

  int ret;

  ret = munmap(shm_addr, mem_size);
  if (ret == -EPERM) {
    std::cout << "Failed to release the memory map." << std::endl;
    return -EPERM;
  }

  ret = shm_unlink(shm_path);
  if (ret == -EPERM) {
    std::cout << "Failed to unlink the shared memory." << std::endl;
    return -EPERM;
  }

  return 0;
}
