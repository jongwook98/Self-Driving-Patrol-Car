/* Copyright 2022. Kim, Jinseong all rights reserved */

#ifndef LIDAR_H
#define LIDAR_H

#include <lane_detection/common/common.h>
#include <lane_detection/common/message_queue.h>
#include <lane_detection/common/thread.h>

#include <sl_lidar.h>
#include <sl_lidar_driver.h>

#include <memory>
#include <mutex>

class Lidar : public Thread {
public: // NOLINT
  Lidar();
  virtual ~Lidar() = default;

private: // NOLINT
  std::unique_ptr<MessageQueue> mq;

  void *Run(void *arg) override;
};

#endif /* LIDAR_H */
