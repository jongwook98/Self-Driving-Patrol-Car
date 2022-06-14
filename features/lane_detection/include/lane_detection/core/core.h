/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef CORE_H
#define CORE_H

#include <lane_detection/camera/camera.h>
#include <lane_detection/lane_detect/lane_detect.h>
#include <lane_detection/traffic_light/traffic_light.h>

#include <csignal>

class Core {
public: // NOLINT
  Core();
  virtual ~Core();

  void Run(void);

private: // NOLINT
  struct sigaction sig_act;

  std::unique_ptr<Camera> cam;
  std::unique_ptr<LaneDetect> lane_detect;
  std::unique_ptr<TrafficLight> traffic_light;

  static inline bool status;
  static void SigIntHandler(int signo);
};

#endif /* CORE_H */
