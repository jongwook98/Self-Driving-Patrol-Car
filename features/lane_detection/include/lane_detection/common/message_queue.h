/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include <mqueue.h>

#include <iostream>

enum class MqMode : uint8_t {
  MQ_START = 0x55,
  MQ_STOP = 0x77,
  LANE_RECOG = 0x01,
  OPENCV = 0x02,
  INTERNAL = 0x04,
  VEHICLE_RECOG = 0x10,
  LIDAR = 0x20,
  DEEPLEARNING = 0x40,
  TOTAL_MQ = 9
};

inline MqMode operator|(const MqMode mode1, const MqMode mode2) {
  return static_cast<MqMode>(static_cast<uint8_t>(mode1) |
                             static_cast<uint8_t>(mode2));
}

class MessageQueue {
public: // NOLINT
  MessageQueue(const char *mq_path, const std::size_t len);
  virtual ~MessageQueue();

  int Send(const char *send_msg, const std::size_t len);
  int Send(const char *send_msg, const std::size_t len, int sec);

  int Receive(char *read_buf, const std::size_t len);
  int Receive(char *read_buf, const std::size_t len, int sec);

  bool BuildMessage(uint8_t *root, const MqMode mode, const uint8_t *data,
                    const std::size_t root_len, const std::size_t data_len);
  bool CheckMessage(MqMode mode, const uint8_t *msg);

  mqd_t GetCurFd(void) const;
  const char *GetMqPath(void) const;

private: // NOLINT
  int Open(const char *mq_path, const std::size_t len);
  int Close();

  mqd_t fd;
  std::size_t len;
  const char *mq_path;
};

#endif /* MESSAGE_QUEUE_H */
