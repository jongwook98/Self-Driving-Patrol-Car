/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef MAIN_H
#define MAIN_H

#include <fcntl.h>
#include <lane_detection/common/message_queue.h>
#include <sys/stat.h>

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "lane_detection/camera/lane_detection.h"
#include "lane_detection/camera/traffic_light.h"

struct message_q {
  uint8_t start;
  uint8_t mode;
  uint8_t status_flag;
  uint8_t angle;
  uint8_t stop;
};

class Lane {
public: // NOLINT
  double rho = 1;
  double theta = CV_PI / 180;
  int threshold = 50;
  double min_line_length = 100;
  double max_line_gap = 100;
  uint8_t data[2] = {0};
  const char *mq_path[1] = {"/lane2"};
  const uint8_t mq_msg_size[1] = {sizeof(uint8_t) * 5};
};
#endif /* MAIN_H */
