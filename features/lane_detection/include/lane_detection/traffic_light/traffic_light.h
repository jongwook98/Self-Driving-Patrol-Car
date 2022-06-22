/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <lane_detection/common/common.h>
#include <lane_detection/common/mqtt.h>
#include <lane_detection/common/thread.h>

#include <memory>

class TrafficLight : public Thread {
public: // NOLINT
  TrafficLight();
  virtual ~TrafficLight();

private: // NOLINT
  char read_buf[MAX_BUF] = {
      0,
  };
  std::unique_ptr<Mqtt> mqtt;

  void *Run(void *arg) override;
};

#endif /* TRAFFIC_LIGHT_H */
