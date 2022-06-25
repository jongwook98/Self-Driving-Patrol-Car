/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/common/common.h>
#include <lane_detection/lane_detect/lane_detect.h>

LaneDetect::LaneDetect()
    : lane_detect_mutex(), mqtt(std::make_unique<Mqtt>(LANE_DETECT_PUB_TOPIC,
                                                       DEFAULT_PORT, nullptr)) {
}

void *LaneDetect::Run(void *arg) {
  const std::string str = LANE_DETECT_PUB_TOPIC;
  std::unique_lock<std::mutex> sync(lane_detect_mutex, std::defer_lock);

  mqtt->Subscribe(LANE_DETECT_SUB_TOPIC, read_buf, sizeof(read_buf));

  while (status) {
    DEBUG_MSG("[Lane Detect] in Thread!");

    /* publish to the traffic light */
    mqtt->Publish(LANE_DETECT_PUB_TOPIC, str.c_str(), str.length());

    sync.lock();
    if (strlen(read_buf)) {
      /* subscribe from the camera */
      WARNING_MSG("Cur: ", str, " from: ", read_buf);
      memset(read_buf, 0x0, sizeof(read_buf));
    }
    sync.unlock();
  }

  return nullptr;
}
