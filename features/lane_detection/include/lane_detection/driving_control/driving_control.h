/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef DRIVING_CONTROL_H
#define DRIVING_CONTROL_H

#include <stdlib.h>

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "lane_detection/camera/lane_detection.h"

class DrivingControl {
private: // NOLINT
  uint8_t stop_flag;
  uint8_t driving_status_flag;
  uint8_t left_flag;
  uint8_t left_stay_flag;
  uint8_t force_straight_stay_flag;
  uint8_t force_straight_flag;

  int stop_cnt;
  int stop_time = 10;
  int left_stay_cnt;
  int left_stay_time = 35;
  int force_straight_stay_cnt;
  int force_straight_stay_time = 35;

  float steering_angle;

public: // NOLINT
  DrivingControl()
      : stop_flag(0), driving_status_flag(0), left_flag(0), left_stay_flag(0),
        force_straight_stay_flag(0), force_straight_flag(0), stop_cnt(0),
        left_stay_cnt(0), force_straight_stay_cnt(0), steering_angle(0.0) {}
  ~DrivingControl() {}
  uint8_t force_flag;
  uint8_t DrivingStatusFlag(int light_color);
  int SteeringAngle(cv::Mat img, uint8_t status);
};

#endif /* DRIVING_CONTROL_H */
