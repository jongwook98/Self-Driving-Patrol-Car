/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/camera/camera.h>

#include <iostream>

Camera::Camera() {}

Camera::~Camera() {}

void *Camera::Run(void *arg) {
  while (status) {
    std::cout << "Camera Thread!" << std::endl;
  }

  return nullptr;
}
