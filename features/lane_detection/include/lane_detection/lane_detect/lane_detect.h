/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef LANE_DETECT_H
#define LANE_DETECT_H

#include <lane_detection/common/thread.h>

class LaneDetect : public Thread {
public: // NOLINT
  LaneDetect();
  virtual ~LaneDetect();

private: // NOLINT
  void *Run(void *arg) override;
};

#endif /* LANE_DETECT_H */
