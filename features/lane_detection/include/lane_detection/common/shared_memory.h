/* Copyright 2022, Kim, Jinseong all rights reserved */

#ifndef SHARED_MEMORY_H
#define SHARED_MEMOTY_H

#include <iostream>

class SharedMemory {
public: // NOLINT
  SharedMemory(const char *shm_path, const std::size_t mem_size);
  virtual ~SharedMemory();

  void ShmCopy(void *shm_path, const void *input_path);

  void *shm_addr;

private: // NOLINT
  int Open(const char *shm_path, const std::size_t mem_size);
  int Close();

  int fd;
  std::size_t mem_size;
  const char *shm_path;
};

#endif /* SHARED_MEMORY_H */
