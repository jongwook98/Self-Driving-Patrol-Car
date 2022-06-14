/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef CAMERA_H
#define CAMERA_H

#include <lane_detection/common/thread.h>

class Camera : public Thread {
public: // NOLINT
  Camera();
  virtual ~Camera();

private: // NOLINT
  void *Run(void *arg) override;
};

#endif /* CAMERA_H */
