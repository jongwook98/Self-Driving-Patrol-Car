/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/common/common.h>
#include <lane_detection/lane_detect/lane_detect.h>

#include <iostream>

LaneDetect::LaneDetect() {}

LaneDetect::~LaneDetect() {}

void *LaneDetect::Run(void *arg) {
  while (status) {
    DEBUG_MSG("[Lane Detect] in Thread!");
  }

  return nullptr;
}
