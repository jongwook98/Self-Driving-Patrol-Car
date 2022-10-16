/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef BINARY_SEMAPHORE_H
#define BINARY_SEMAPHORE_H

#include <semaphore.h>

enum class SemaMode : int { BINARY_SEMAPHORE = 1 };

class BinarySemaphore {
public: // NOLINT
  explicit BinarySemaphore(const char *bsp_path);
  ~BinarySemaphore();

  void Lock(void);
  void Unlock(void);

private: // NOLINT
  int Open(void);
  int Close(void);

  sem_t *bsp;
  const char *bsp_path;
};

#endif
