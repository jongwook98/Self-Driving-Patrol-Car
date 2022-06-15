/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/common/common.h>
#include <lane_detection/core/core.h>

#include <unistd.h>

#include <csignal>
#include <iostream>

Core::Core()
    : sig_act({Core::SigIntHandler, 0, 0, 0, 0}),
      cam(std::make_unique<Camera>()),
      lane_detect(std::make_unique<LaneDetect>()),
      traffic_light(std::make_unique<TrafficLight>()) {
  Core::status = true;

  sigemptyset(&sig_act.sa_mask);
  sigaction(SIGINT, &sig_act, NULL);
}

Core::~Core() {
  Core::status = false;

  traffic_light = nullptr;
  lane_detect = nullptr;
  cam = nullptr;
}

void Core::Run(void) {
  DEBUG_MSG("[core] Start!");
  cam->Start(nullptr);
  lane_detect->Start(nullptr);
  traffic_light->Start(nullptr);

  while (Core::status) {
    sleep(1);
  }

  traffic_light->Stop();
  lane_detect->Stop();
  cam->Stop();
  DEBUG_MSG("[core] end");
}

void Core::SigIntHandler(int signo) {
  (void)signo;
  Core::status = false;
}
