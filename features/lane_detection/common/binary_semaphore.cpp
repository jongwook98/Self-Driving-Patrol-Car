/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/common/binary_semaphore.h>
#include <lane_detection/common/common.h>

#include <fcntl.h>
#include <sys/stat.h>

BinarySemaphore::BinarySemaphore(const char *bsp_path)
    : bsp(nullptr), bsp_path(bsp_path) {
  int ret = Open();
  EXIT_GUARD(ret < 0, "[BSP] in Constructor");
}

BinarySemaphore::~BinarySemaphore() { Close(); }

void BinarySemaphore::Lock(void) { sem_wait(bsp); }

void BinarySemaphore::Unlock(void) { sem_post(bsp); }

int BinarySemaphore::Open(void) {
  bsp = sem_open(bsp_path, O_RDWR | O_CREAT | O_EXCL, 0666,
                 static_cast<int>(SemaMode::BINARY_SEMAPHORE));
  FORMULA_GUARD(bsp == nullptr, -EPERM, "Failed to open the semaphore.");

  return 0;
}

int BinarySemaphore::Close(void) {
  sem_close(bsp);
  sem_unlink(bsp_path);

  return 0;
}
