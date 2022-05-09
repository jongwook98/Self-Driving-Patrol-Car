/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <opencv2/imgproc/types_c.h>

#include "lane_detection/camera/lane_detection.h"

class TrafficLight {
private: // NOLINT
  int light_color;
  double sigma_color;
  double sigma_space;

public: // NOLINT
  TrafficLight() : light_color(0), sigma_color(10), sigma_space(10) {}
  ~TrafficLight() {}
  int FindTrafficLight(cv::Mat img);
};

#endif /* TRAFFIC_LIGHT_H */
