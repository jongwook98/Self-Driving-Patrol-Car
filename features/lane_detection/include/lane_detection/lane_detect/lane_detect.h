/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef LANE_DETECT_H
#define LANE_DETECT_H

#include <lane_detection/common/common.h>
#include <lane_detection/common/message_queue.h>
#include <lane_detection/common/mqtt.h>
#include <lane_detection/common/thread.h>

#include <memory>
#include <mutex>
#include <vector>

#include "opencv2/opencv.hpp"

#define LANE_PATH "/lane_detection_to_core"

#define MQ_SIZE sizeof(uint8_t) * 14
#define INIT 0
#define WIDTH 640
#define HEIGHT 360
#define MID 250
#define HO_BOUNDARY 200
#define LINE_THICKNESS 2
#define LINE_TYPE 8
#define LINE_SHIFT 0
#define PIXEL_D 5
#define SIGMACOLOR 100
#define SIGMASPACE 100
#define RHO 1
#define CANNY_LOW_THRESHOLD 150
#define CANNY_HIGH_THRESHOLD 270
#define THETA CV_PI / 180
#define THRESHOLD 50
#define MAX_SLOPE 999.
#define MIN_LINE_LENGTH 5
#define MAX_LINE_GAP 100
#define LINE_ROW_THRESHOLD tan(15 * CV_PI / 180)
#define LINE_HIGH_THRESHOLD tan(89.9 * CV_PI / 180)
#define left_line 100
#define right_line 530
#define MID_ANGLE 90
#define SLOPE_THRESHOLD 1.2
#define X_THRESHOLD 200
#define X_HO_THRESHOLD 150
#define BLUE cv::Scalar(255, 0, 0)
#define RED cv::Scalar(0, 0, 255)
#define GREEN cv::Scalar(0, 255, 0)
#define LOWER_HSV cv::Scalar(10, 30, 100)
#define UPPER_HSV cv::Scalar(60, 255, 255)
#define STOP_NUM 1000

struct lane_message_q {
  uint8_t start;
  uint8_t mode;
  uint8_t jump;
  uint8_t angle;
  uint8_t stopinf[8];
  uint8_t line_inf;
  uint8_t stop;
};

class LaneDetect : public Thread {
public: // NOLINT
  LaneDetect();
  virtual ~LaneDetect() = default;

private: // NOLINT
  char read_buf[MAX_BUF] = {
      0,
  };
  std::mutex lane_detect_mutex;
  std::unique_ptr<Mqtt> mqtt;
  std::unique_ptr<MessageQueue> mq;
  void *Run(void *arg) override;
  cv::Mat SetRoi(cv::Mat img, cv::Point *vertices);
  int CalAngle(float servo_direct, const cv::Mat &line_result);
  int CalLanes(cv::Mat img, const cv::Mat &line_result,
               std::vector<cv::Vec4i> lines);
  int CalLaneAngle(cv::Mat src);
  int FindLastPoint(float servo_direct, const cv::Mat &line_result);
  float DivideLane(std::vector<cv::Vec4i> lines, const cv::Mat &line_result);
  cv::Mat FindColorHsv(cv::Mat img);
  cv::Mat ConvertImg(cv::Mat src);
  int FindLanes(cv::Mat src, cv::Mat canny);
  int SendMQ(int angle);
  int l_x, r_x;
  int line_flag[2];
  int pre_c_x2;
  float r_slope, l_slope;
  double stopline;
  int change_line;
};

#endif /* LANE_DETECT_H */
