/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/common/common.h>
#include <lane_detection/lane_detect/lane_detect.h>

LaneDetect::LaneDetect()
    : mqtt(std::make_unique<Mqtt>(LANE_DETECT_PUB_TOPIC, DEFALUT_PORT,
                                  nullptr)) {}

LaneDetect::~LaneDetect() { mqtt = nullptr; }

void *LaneDetect::Run(void *arg) {
  mqtt->Subscribe(LANE_DETECT_SUB_TOPIC, read_buf, sizeof(read_buf));

  std::string str = LANE_DETECT_PUB_TOPIC;
  while (status) {
    DEBUG_MSG("[Lane Detect] in Thread!");

    /* publish to the traffic light */
    mqtt->Publish(LANE_DETECT_PUB_TOPIC, str.c_str(), str.length());

    /* subscribe from the camera */
    if (strlen(read_buf)) {
      WARNING_MSG("Cur: ", str, " from: ", read_buf);
      memset(read_buf, 0x0, sizeof(read_buf));
    }
  }

  return nullptr;
}
