/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef LANE_DETECTION_H
#define LANE_DETECTION_H

#include <lane_detection/main.h>

#include <fcntl.h>
#include <stdlib.h>

#include <cmath>
#include <functional>
#include <vector>

#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

class LaneDetection {
private: // NOLINT
  int width;
  int l_x1, l_x2, r_x1, r_x2;
  int line_flag[2];
  int c_x1;
  int c_y1;
  int c_y2;
  int c_x2;
  int pre_c_x2;

  float slope;
  float r_slope, l_slope;
  float diff;
  float calculated_angle;

  // LaneDetection
  double rho = 1;
  double theta = CV_PI / 180;
  int threshold = 50;
  double min_line_length = 25;
  double max_line_gap = 100;

public: // NOLINT
  LaneDetection()
      : width(800), l_x1(0), l_x2(0), r_x1(0), r_x2(0), line_flag{0, 0},
        c_x1(400), c_y1(600), c_y2(0), c_x2(0), slope(0.0), r_slope(0),
        l_slope(0), diff(0), calculated_angle(0), pre_c_x2(540) {}
  ~LaneDetection() {}
  cv::Mat RegionOfInterset(cv::Mat img, cv::Point *vertices);
  cv::Mat FindColorHsv(cv::Mat img);
  int EdgeLines(cv::Mat img, const cv::Mat &line_result,
                std::vector<cv::Vec4i> lines);
  int CalculationAngle(float servo_direct, int line_flag[],
                       const cv::Mat &line_result, int l_x2, int r_x1);
  int StraightLaneAngle(cv::Mat img);
};

#endif /* LANE_DETECTION_H */
