/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/common/common.h>
#include <lane_detection/traffic_light/traffic_light.h>

TrafficLight::TrafficLight()
    : traffic_light_mutex(),
      mqtt(std::make_unique<Mqtt>(TRAFFIC_LIGHT_PUB_TOPIC, DEFAULT_PORT,
                                  nullptr)) {}

void *TrafficLight::Run(void *arg) {
  const std::string str = TRAFFIC_LIGHT_PUB_TOPIC;
  std::unique_lock<std::mutex> sync(traffic_light_mutex, std::defer_lock);

  mqtt->Subscribe(TRAFFIC_LIGHT_SUB_TOPIC, read_buf, sizeof(read_buf));

  while (status) {
    DEBUG_MSG("[Traffic Light] in Thread!");

    /* publish to the camera */
    mqtt->Publish(TRAFFIC_LIGHT_PUB_TOPIC, str.c_str(), str.length());

    sync.lock();
    if (strlen(read_buf)) {
      /* subscribe from the lane detect */
      WARNING_MSG("Cur: ", str, " from: ", read_buf);
      memset(read_buf, 0x0, sizeof(read_buf));
    }
    sync.unlock();
  }

  return nullptr;
}
