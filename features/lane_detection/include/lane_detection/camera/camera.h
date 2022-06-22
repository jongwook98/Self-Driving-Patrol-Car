/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef CAMERA_H
#define CAMERA_H

#include <lane_detection/common/common.h>
#include <lane_detection/common/mqtt.h>
#include <lane_detection/common/thread.h>

#include <memory>

class Camera : public Thread {
public: // NOLINT
  Camera();
  virtual ~Camera();

private: // NOLINT
  char read_buf[MAX_BUF] = {
      0,
  };
  std::unique_ptr<Mqtt> mqtt;

  void *Run(void *arg) override;
};

#endif /* CAMERA_H */
