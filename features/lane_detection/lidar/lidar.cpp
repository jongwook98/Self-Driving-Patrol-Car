/* Copyright 2022. Kim, Jinseong all rights reserved */

#include <lane_detection/common/common.h>
#include <lane_detection/lidar/lidar.h>

#define LIDAR_PATH "/lidar_data_to_core"

Lidar::Lidar()
    : mq(std::make_unique<MessageQueue>(LIDAR_PATH, (sizeof(uint8_t) * 5)) ) {}

void *Lidar::Run(void *arg) {
  while (status) {
    DEBUG_MSG("[LIDAR] in Thread!");
  }

  return nullptr;
}
