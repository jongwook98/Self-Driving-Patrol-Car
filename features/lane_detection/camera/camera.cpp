/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/camera/camera.h>

Camera::Camera()
    : camera_mutex(),
      mqtt(std::make_unique<Mqtt>(CAMERA_PUB_TOPIC, DEFAULT_PORT, nullptr)) {}

void *Camera::Run(void *arg) {
  const std::string str = CAMERA_PUB_TOPIC;
  std::unique_lock<std::mutex> sync(camera_mutex, std::defer_lock);

  mqtt->Subscribe(CAMERA_SUB_TOPIC, read_buf, sizeof(read_buf));

  while (status) {
    DEBUG_MSG("[Camera] in Thread!");

    /* publish to the lane detect */
    mqtt->Publish(CAMERA_PUB_TOPIC, str.c_str(), str.length());

    sync.lock();
    if (strlen(read_buf)) {
      /* subscribe from the traffic light */
      WARNING_MSG("Cur: ", str, " from: ", read_buf);
      memset(read_buf, 0x0, sizeof(read_buf));
    }
    sync.unlock();
  }

  return nullptr;
}
