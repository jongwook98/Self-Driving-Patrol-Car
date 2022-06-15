/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/camera/camera.h>
#include <lane_detection/common/common.h>

Camera::Camera() {}

Camera::~Camera() {}

void *Camera::Run(void *arg) {
  while (status) {
    DEBUG_MSG("[Camera] in Thread!");
  }

  return nullptr;
}
