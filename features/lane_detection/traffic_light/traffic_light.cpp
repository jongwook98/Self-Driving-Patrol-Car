/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/common/common.h>
#include <lane_detection/traffic_light/traffic_light.h>

TrafficLight::TrafficLight()
    : mqtt(std::make_unique<Mqtt>(TRAFFIC_LIGHT_PUB_TOPIC, DEFALUT_PORT,
                                  nullptr)) {}

TrafficLight::~TrafficLight() { mqtt = nullptr; }

void *TrafficLight::Run(void *arg) {
  mqtt->Subscribe(TRAFFIC_LIGHT_SUB_TOPIC, read_buf, sizeof(read_buf));

  std::string str = TRAFFIC_LIGHT_PUB_TOPIC;
  while (status) {
    DEBUG_MSG("[Traffic Light] in Thread!");

    /* publish to the camera */
    mqtt->Publish(TRAFFIC_LIGHT_PUB_TOPIC, str.c_str(), str.length());

    /* subscribe from the lane detect */
    if (strlen(read_buf)) {
      WARNING_MSG("Cur: ", str, " from: ", read_buf);
      memset(read_buf, 0x0, sizeof(read_buf));
    }
  }

  return nullptr;
}
