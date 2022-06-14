/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/lane_detect/lane_detect.h>

#include <iostream>

LaneDetect::LaneDetect() {}

LaneDetect::~LaneDetect() {}

void *LaneDetect::Run(void *arg) {
  while (status) {
    std::cout << "Lane Detect Thread!" << std::endl;
  }

  return nullptr;
}
