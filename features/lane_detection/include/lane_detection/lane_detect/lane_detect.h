/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef LANE_DETECT_H
#define LANE_DETECT_H

#include <lane_detection/common/common.h>
#include <lane_detection/common/mqtt.h>
#include <lane_detection/common/thread.h>

#include <memory>
#include <mutex>

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

  void *Run(void *arg) override;
};

#endif /* LANE_DETECT_H */
