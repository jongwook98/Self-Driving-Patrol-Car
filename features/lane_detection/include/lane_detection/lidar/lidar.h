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
  struct message_q {
    uint8_t start;
    uint8_t mode;
    uint8_t trash;
    uint8_t obstacle_num;
    uint8_t obstacle_info[40];
    uint8_t stop;
  };

  struct send_data {
    uint16_t min_dis;
    uint16_t min_angle;
  };

  struct obstacle {
    float start_angle;
    float end_angle;
    float min_dis;
    float min_angle;

    struct send_data s_data;
  };

  float left_start = 0;
  // 90, 270, 360 degree raw data
  float left_end = 16384;
  float right_start = 49152;
  float right_end = 65536;

  float min_dis_mm = 0;
  float max_dis_mm = 2550;
  float same_ob = 750;
  float same_ob_start = 500;
  float same_ob_opposite = 65035;

  sl::ILidarDriver *Init();
  void Exit(sl::ILidarDriver *drv);
  void Operate(sl::ILidarDriver *dobstaclerv, struct Lidar::obstacle *ob);
  int Publisher(int number, struct Lidar::obstacle *ob);
  int SendMQ(int number, struct Lidar::obstacle *ob);

  void *Run(void *arg) override;
};

#endif /* LIDAR_H */
