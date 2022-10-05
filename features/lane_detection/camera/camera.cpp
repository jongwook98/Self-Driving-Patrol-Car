/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/camera/camera.h>

Camera::Camera()
    : camera_mutex(),
      mqtt(std::make_unique<Mqtt>(CAMERA_PUB_TOPIC, DEFAULT_PORT, nullptr)) {}

cv::VideoCapture Camera::StartCamera() {
    cv::VideoCapture cap(2);
    // cv::VideoCapture cap(cv::CAP_V4L2);
    FORMULA_GUARD(!cap.isOpened(), false, CAMERA_OPEN_ERR_MSG);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, CAMERA_WIDTH);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, CAMERA_HEIGHT);
    return cap;
}

int Camera::SetCalibration() {
    intrinsic_read = cv::Mat(3, 3, 6);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            intrinsic_read.ptr<double>(i)[j] = intrinsic[i][j];

    dist_coeffs_read = cv::Mat(1, 5, 6);
    for (int i = 0; i < 1; i++)
        for (int j = 0; j < 5; j++)
            dist_coeffs_read.ptr<double>(i)[j] = dist_coeffs[i][j];
    return 0;
}
cv::Mat Camera::Calibration(cv::Mat img) {
    cv::Mat src;
    cv::undistort(img, src, intrinsic_read, dist_coeffs_read);
    cv::resize(img, img, cv::Size(), X_SIZE, Y_SIZE);
    cv::resize(src, src, cv::Size(), X_SIZE, Y_SIZE);

    return src;
}

void *Camera::Run(void *arg) {
  std::unique_lock<std::mutex> sync(camera_mutex, std::defer_lock);
  cv::VideoCapture cam = StartCamera();
  SetCalibration();
  mqtt->Subscribe(CAMERA_SUB_TOPIC, read_buf, sizeof(read_buf));
  cv::Mat img;
  cv::Mat src;
  while (status) {
    DEBUG_MSG("[Camera] in Thread!");
    cam >> img;
    if (cv::waitKey(1) == 27)
      break;
    src = Calibration(img);
    /* publish to the lane detect */
    mqtt->Publish(CAMERA_PUB_TOPIC, reinterpret_cast<void *>(&src),
                  sizeof(src));
    mqtt->Publish(LANE_DETECT_PUB_TOPIC, reinterpret_cast<void *>(&src),
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
