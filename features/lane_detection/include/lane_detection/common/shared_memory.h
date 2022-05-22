/* Copyright 2022, Kim, Jinseong all rights reserved */

#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <iostream>

class SharedMemory {
public: // NOLINT
  SharedMemory(const char *shm_path, const off_t mem_size);
  virtual ~SharedMemory();

  int CopyToMem(const void *write_buf, off_t offset);
  int CopyFromMem(void *read_buf, off_t offset);

private: // NOLINT
  int Open(const char *shm_path, const off_t mem_size);
  int Close(void);

  int fd;
  off_t req_size;

  void *virt_addr;
  const char *shm_path;
};

#endif /* SHARED_MEMORY_H */
