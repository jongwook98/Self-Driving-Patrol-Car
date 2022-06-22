/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/camera/camera.h>

Camera::Camera()
    : mqtt(std::make_unique<Mqtt>(CAMERA_PUB_TOPIC, DEFALUT_PORT, nullptr)) {}

Camera::~Camera() { mqtt = nullptr; }

void *Camera::Run(void *arg) {
  mqtt->Subscribe(CAMERA_SUB_TOPIC, read_buf, sizeof(read_buf));

  std::string str = CAMERA_PUB_TOPIC;
  while (status) {
    DEBUG_MSG("[Camera] in Thread!");

    /* publish to the lane detect */
    mqtt->Publish(CAMERA_PUB_TOPIC, str.c_str(), str.length());

    /* subscribe from the traffic light */
    if (strlen(read_buf)) {
      WARNING_MSG("Cur: ", str, " from: ", read_buf);
      memset(read_buf, 0x0, sizeof(read_buf));
    }
  }

  return nullptr;
}
