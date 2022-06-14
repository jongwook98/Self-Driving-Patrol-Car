/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <lane_detection/common/thread.h>

class TrafficLight : public Thread {
public: // NOLINT
  TrafficLight();
  virtual ~TrafficLight();

private: // NOLINT
  void *Run(void *arg) override;
};

#endif /* TRAFFIC_LIGHT_H */
