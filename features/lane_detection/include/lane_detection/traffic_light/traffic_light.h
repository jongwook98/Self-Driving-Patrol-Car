/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <lane_detection/common/common.h>
#include <lane_detection/common/message_queue.h>
#include <lane_detection/common/mqtt.h>
#include <lane_detection/common/thread.h>
#include <opencv2/imgproc/types_c.h>

#include <memory>
#include <mutex>
#include <vector>

#include "opencv2/opencv.hpp"

#define TRAFFIC_PATH "/traffic_light_to_core"

#define G_RATIO 0.6
#define sigma_color 10
#define sigma_space 10
#define font_face 2
#define font_scale 1.2

class TrafficLight : public Thread {
public: // NOLINT
  TrafficLight();
  virtual ~TrafficLight() = default;

private: // NOLINT
  char read_buf[MAX_BUF] = {
      0,
  };
  std::mutex traffic_light_mutex;
  std::unique_ptr<Mqtt> mqtt;
  std::unique_ptr<MessageQueue> mq;
  struct message_q {
    uint8_t start;
    uint8_t mode;
    uint8_t color;
    uint8_t stop;
  };

  cv::Mat RegionOfInterset(cv::Mat cam);
  void ShowResult(cv::Mat result, int color);
  int TotalPixel(std::vector<cv::Vec3f> circles, size_t i);
  int CountGreen(cv::Mat cam, std::vector<cv::Vec3f> circles, size_t i);
  int GetColor(cv::Mat cam, cv::Mat gray_img, std::vector<cv::Vec3f> circles,
               size_t i);
  void Operate(cv::Mat cam);
  int Publisher(int color);
  void *Run(void *arg) override;
};

#endif /* TRAFFIC_LIGHT_H */
