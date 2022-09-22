/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/camera/camera.h>

Camera::Camera()
    : camera_mutex(),
      mqtt(std::make_unique<Mqtt>(CAMERA_PUB_TOPIC, DEFAULT_PORT, nullptr)) {}

cv::VideoCapture Camera::StartCamera() {
    // cv::VideoCapture cap(2);
    cv::VideoCapture cap(cv::CAP_V4L2);
    if (!cap.isOpened()) {
        printf("Can't open the camera");
    }
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 800);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 600);
    return cap;
}

void *Camera::Run(void *arg) {
  // const std::string str = CAMERA_PUB_TOPIC;
  std::unique_lock<std::mutex> sync(camera_mutex, std::defer_lock);
  cv::VideoCapture cam = StartCamera();
  mqtt->Subscribe(CAMERA_SUB_TOPIC, read_buf, sizeof(read_buf));
  cv::Mat src;
  while (status) {
    DEBUG_MSG("[Camera] in Thread!");
    cam >> src;
    if (cv::waitKey(1) == 27)
      break;
    /* publish to the lane detect */
    mqtt->Publish(CAMERA_PUB_TOPIC, reinterpret_cast<void *>(&src),
                  sizeof(src));
    sync.lock();
    if (strlen(read_buf)) {
      /* subscribe from the traffic light */
      memset(read_buf, 0x0, sizeof(read_buf));
    }
    sync.unlock();
  }
  return nullptr;
}
