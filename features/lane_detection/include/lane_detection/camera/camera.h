/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef CAMERA_H
#define CAMERA_H

#include <lane_detection/common/common.h>
#include <lane_detection/common/mqtt.h>
#include <lane_detection/common/thread.h>

#include <fstream>
#include <memory>
#include <mutex>

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#define CAMERA_OPEN_ERR_MSG "Can't open the camera"
#define CALIBRATION_OPEN_ERR_MSG "Can't open the file"

#define CAMERA_WIDTH 1280
#define CAMERA_HEIGHT 720
#define X_SIZE 0.5
#define Y_SIZE 0.5
#define intrinsic_0_0 505.103
#define intrinsic_0_1 0
#define intrinsic_0_2 613.081
#define intrinsic_1_0 0
#define intrinsic_1_1 500.032
#define intrinsic_1_2 476.193
#define intrinsic_2_0 0
#define intrinsic_2_1 0
#define intrinsic_2_2 1
#define dist_0 -0.375055
#define dist_1 0.115115
#define dist_2 -0.00291268
#define dist_3 0.00123298
#define dist_4 -0.0145697

class Camera : public Thread {
public: // NOLINT
  Camera();
  virtual ~Camera() = default;

private: // NOLINT
  char read_buf[MAX_BUF] = {
      0,
  };
  double intrinsic[3][3] = {intrinsic_0_0, intrinsic_0_1, intrinsic_0_2,
                            intrinsic_1_0, intrinsic_1_1, intrinsic_1_2,
                            intrinsic_2_0, intrinsic_2_1, intrinsic_2_2};
  double dist_coeffs[1][5] = {dist_0, dist_1, dist_2, dist_3, dist_4};
  cv::VideoCapture StartCamera();
  cv::Mat Calibration(cv::Mat img);
  int SetCalibration();
  std::mutex camera_mutex;
  std::unique_ptr<Mqtt> mqtt;
  void *Run(void *arg) override;
  cv::Mat intrinsic_read;
  cv::Mat dist_coeffs_read;
};

#endif /* CAMERA_H */
