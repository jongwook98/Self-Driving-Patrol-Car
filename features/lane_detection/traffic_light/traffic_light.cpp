/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/common/common.h>
#include <lane_detection/traffic_light/traffic_light.h>

#include <iostream>

TrafficLight::TrafficLight() {}

TrafficLight::~TrafficLight() {}

void *TrafficLight::Run(void *arg) {
  while (status) {
    DEBUG_MSG("[Traffic Light] in Thread!");
  }

  return nullptr;
}
