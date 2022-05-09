/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef LANE_DETECTION_H
#define LANE_DETECTION_H

#include <fcntl.h>
#include <stdlib.h>

#include <cmath>
#include <functional>
#include <vector>

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "lane_detection/main.h"

class LaneDetection {
private: // NOLINT
  int width;
  int l_x1, l_x2, r_x1, r_x2;
  int line_flag[2];
  int c_x1;
  int c_y1;
  int c_y2;
  int c_x2;

  float slope;
  float r_slope, l_slope;
  float diff;
  float steering_angle;

public: // NOLINT
  LaneDetection()
      : width(800), l_x1(0), l_x2(0), r_x1(0), r_x2(0), line_flag{0, 0},
        c_x1(400), c_y1(600), c_y2(0), c_x2(0), slope(0.0), r_slope(0),
        l_slope(0), diff(0), steering_angle(0) {}
  ~LaneDetection() {}
  cv::Mat RegionOfInterset(cv::Mat img, cv::Point *vertices);
  cv::Mat FindColorHsv(cv::Mat img);
  float EdgeLines(cv::Mat img, const cv::Mat &line_result,
                  std::vector<cv::Vec4i> lines);
  float CalculationAngle(float servo_direct, int line_flag[],
                         const cv::Mat &line_result, int l_x2, int r_x1);
};

#endif /* LANE_DETECTION_H */
