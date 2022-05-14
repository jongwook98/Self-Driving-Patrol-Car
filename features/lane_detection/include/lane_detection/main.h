/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef MAIN_H
#define MAIN_H

#include <lane_detection/camera/lane_detection.h>
#include <lane_detection/camera/traffic_light.h>
#include <lane_detection/common/message_queue.h>
#include <lane_detection/driving_control/driving_control.h>

#include <fcntl.h>
#include <sys/stat.h>

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

struct message_q {
  uint8_t start;
  uint8_t mode;
  uint8_t status_flag;
  uint8_t angle;
  uint8_t stop;
};

class Lane {
public: // NOLINT
  uint8_t steering_angle;
  uint8_t traffic_light;
  uint8_t driving_status_flag;
  uint8_t data[2] = {0};
  const char *mq_path[1] = {"/lane2"};
  const uint8_t mq_msg_size[1] = {sizeof(uint8_t) * 5};
};
#endif /* MAIN_H */
