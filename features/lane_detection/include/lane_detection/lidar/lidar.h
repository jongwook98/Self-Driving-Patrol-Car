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

#ifndef _countof
#define _countof(_Array) static_cast<int>(sizeof(_Array) / sizeof(_Array[0]))
#endif

class Lidar : public Thread {
public: // NOLINT
  Lidar();
  virtual ~Lidar() = default;

private: // NOLINT
  std::unique_ptr<MessageQueue> mq;

  struct obstacle {
    float start_angle;
    float end_angle;
    float min_dis;
    float min_angle;
  };

  float min_dis_mm = 0;
  float max_dis_mm = 1000;
  float same_ob_start = 1000;
  float same_ob_opposite = 64625;

  sl::ILidarDriver *Init();
  void Exit(sl::ILidarDriver *drv);
  void Operate(sl::ILidarDriver *dobstaclerv, struct Lidar::obstacle *ob);
  int Publisher(int number, struct Lidar::obstacle *ob);

  void *Run(void *arg) override;
};

#endif /* LIDAR_H */
