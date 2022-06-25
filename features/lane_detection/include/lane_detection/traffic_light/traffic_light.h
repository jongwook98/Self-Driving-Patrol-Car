/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <lane_detection/common/common.h>
#include <lane_detection/common/mqtt.h>
#include <lane_detection/common/thread.h>

#include <memory>
#include <mutex>

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

  void *Run(void *arg) override;
};

#endif /* TRAFFIC_LIGHT_H */
