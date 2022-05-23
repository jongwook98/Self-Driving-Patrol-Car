/*
 * Copyright 2022. Park, Sangjae/Bae, Youwon/Kim, Jinseong all rights reserved
 */
#include <lane_detection/camera/lane_detection.h>
#include <lane_detection/camera/traffic_light.h>
#include <lane_detection/common/message_queue.h>
#include <lane_detection/common/shared_memory.h>
#include <lane_detection/driving_control/driving_control.h>

#include <fcntl.h>
#include <sys/stat.h>

#include <csignal>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#define SHARED_MEMORY_SIZE 1769472

static int g_escape_flag = 1;

static void sig_ctrl_c(int signo) {
  static_cast<void>(signo);

  std::perror("Input Ctrl+C");
  g_escape_flag = 0;
}

struct message {
  uint8_t start;
  uint8_t mode;
  uint8_t status;
  uint8_t angle;
  uint8_t stop;
} __attribute__((packed));

class LaneDetectionMain {
public: // NOLINT
  LaneDetectionMain();
  virtual ~LaneDetectionMain();

  /* getter */
  cv::VideoCapture GetCap(void) const;
  std::shared_ptr<TrafficLight> GetTrafficLight(void) const;
  std::shared_ptr<DrivingControl> GetControl(void) const;
  std::shared_ptr<MessageQueue> GetMq(void) const;
  std::shared_ptr<SharedMemory> GetShm(void) const;

private: // NOLINT
  cv::VideoCapture cap;

  std::shared_ptr<TrafficLight> light;
  std::shared_ptr<DrivingControl> control;
  std::shared_ptr<MessageQueue> mq;
  std::shared_ptr<SharedMemory> shm;
};

LaneDetectionMain::LaneDetectionMain()
    : cap(cv::CAP_V4L2), light(std::make_shared<TrafficLight>()),
      control(std::make_shared<DrivingControl>()),
      mq(std::make_shared<MessageQueue>("/lane_detection_to_core",
                                        sizeof(struct message))),
      shm(std::make_shared<SharedMemory>("shm", SHARED_MEMORY_SIZE)) {
  cap.set(cv::CAP_PROP_FRAME_WIDTH, 1080);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, 600);
  if (!cap.isOpened()) {
    std::cout << "Failed to open the camera." << std::endl;
    exit(-EPERM);
  }
}

LaneDetectionMain::~LaneDetectionMain() {
  light = nullptr;
  control = nullptr;
  mq = nullptr;
  shm = nullptr;
}

cv::VideoCapture LaneDetectionMain::GetCap(void) const { return this->cap; }

std::shared_ptr<TrafficLight> LaneDetectionMain::GetTrafficLight(void) const {
  return this->light;
}

std::shared_ptr<DrivingControl> LaneDetectionMain::GetControl(void) const {
  return this->control;
}

std::shared_ptr<MessageQueue> LaneDetectionMain::GetMq(void) const {
  return this->mq;
}

std::shared_ptr<SharedMemory> LaneDetectionMain::GetShm(void) const {
  return this->shm;
}

int main() {
  struct sigaction sig_act;
  sig_act.sa_handler = sig_ctrl_c;
  sig_act.sa_flags = 0;

  sigemptyset(&sig_act.sa_mask);
  sigaction(SIGINT, &sig_act, NULL);

  LaneDetectionMain ld_main;

  cv::Mat cam;
  uint8_t send_data[2] = {
      0,
  };
  const std::string light_table[] = {"nothing", "red_light", "green_light",
                                     "left_light"};
  struct message lane_msg;

  while (g_escape_flag) {
    ld_main.GetCap() >> cam;
    if (cv::waitKey(1) == 27)
      break;

    int traffic_light = ld_main.GetTrafficLight()->FindTrafficLight(cam);
    uint8_t driving_status = ld_main.GetControl()->
                             DrivingStatusFlag(traffic_light);
    uint8_t steering_angle = ld_main.GetControl()->
                             SteeringAngle(cam, driving_status);
    send_data[0] = driving_status;
    send_data[1] = steering_angle + 90;

    ld_main.GetShm()->CopyToMem(cam.data, SHARED_MEMORY_SIZE);

    memset(&lane_msg, 0x0, sizeof(struct message));
    ld_main.GetMq()->BuildMessage(reinterpret_cast<uint8_t *>(&lane_msg),
                                  (MqMode::LANE_RECOG | MqMode::OPENCV),
                                  send_data, sizeof(struct message),
                                  sizeof(send_data));
    ld_main.GetMq()->Send(reinterpret_cast<const char *>(&lane_msg),
                          sizeof(struct message));

    std::cout << "T_light: " << light_table[traffic_light] << " | ";
    std::cout << "status_flag: " << driving_status << " | ";
    std::cout << "sterring_angle: " << steering_angle << std::endl;
  }

  std::cout << "LaneDetection Done!" << std::endl;
  return 0;
}
