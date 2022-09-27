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

#define WIDTH 800
#define HEIGHT 600
#define RHO 1
#define THETA CV_PI / 180
#define THRESHOLD 50
#define MIN_LINE_LENGTH 25
#define MAX_LINE_GAP 100
#define LINE_ROW_THRESHOLD tan(15 * CV_PI / 180)
#define LINE_HIGH_THRESHOLD tan(89.9 * CV_PI / 180)
#define LOWER_HSV cv::Scalar(0, 0, 200)
#define UPPER_HSV cv::Scalar(180, 50, 245)

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
  struct message_q {
    uint8_t start;
    uint8_t mode;
    uint8_t jump;
    uint8_t stopinf[8];
    uint8_t angle;
    uint8_t line_inf;
    uint8_t stop;
  };
  void *Run(void *arg) override;
  cv::Mat SetRoi(cv::Mat img, cv::Point *vertices);
  int CalAngle(float servo_direct, const cv::Mat &line_result);
  int CalLanes(cv::Mat img, const cv::Mat &line_result,
               std::vector<cv::Vec4i> lines);
  int CalLaneAngle(cv::Mat src);
  int FindLastPoint(float servo_direct, const cv::Mat &line_result);
  float DivideLane(std::vector<cv::Vec4i> lines);
  cv::Mat FindColorHsv(cv::Mat img);
  cv::Mat ConvertImg(cv::Mat src);
  int FindLanes(cv::Mat src, cv::Mat canny);
  int SendMQ(int angle);
  int l_x, r_x;
  int line_flag[2];
  int pre_c_x2;
  float r_slope, l_slope;
  double stopline;
};

#endif /* LANE_DETECT_H */
