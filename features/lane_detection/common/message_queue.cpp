/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/common/common.h>
#include <lane_detection/common/message_queue.h>

#include <fcntl.h>
#include <sys/stat.h>

#include <cstring>

#define ERR_SEND_FAIL "Failed to send the message."
#define ERR_RECEIVE_FAIL "Failed to receive the message."

MessageQueue::MessageQueue(const char *mq_path, const std::size_t len)
    : fd(static_cast<mqd_t>(-1)), len(len), mq_path(mq_path) {
  int ret = Open(mq_path, len);
  EXIT_GUARD(ret < 0, "[MQ] in Constructor");
}

MessageQueue::~MessageQueue() { Close(); }

int MessageQueue::Send(const char *send_msg, const std::size_t len) {
  int ret = mq_send(fd, send_msg, len, 0);
  FORMULA_GUARD(ret < 0, -EPERM, ERR_SEND_FAIL);

  return ret;
}

int MessageQueue::Send(const char *send_msg, const std::size_t len, int sec) {
  struct timespec limit;
  clock_gettime(CLOCK_REALTIME, &limit);
  limit.tv_sec += sec;

  int ret = mq_timedsend(fd, send_msg, len, 0, &limit);
  FORMULA_GUARD(ret < 0, -EPERM, ERR_SEND_FAIL);

  return ret;
}

int MessageQueue::Receive(char *read_buf, const std::size_t len) {
  ssize_t ret = mq_receive(fd, read_buf, len, NULL);
  FORMULA_GUARD(ret < 0, -EPERM, ERR_RECEIVE_FAIL);

  return static_cast<int>(ret);
}

int MessageQueue::Receive(char *read_buf, const std::size_t len, int sec) {
  struct timespec limit;
  clock_gettime(CLOCK_REALTIME, &limit);
  limit.tv_sec += sec;

  ssize_t ret = mq_timedreceive(fd, read_buf, len, NULL, &limit);
  FORMULA_GUARD(ret < 0, -EPERM, ERR_RECEIVE_FAIL);

  return static_cast<int>(ret);
}

bool MessageQueue::BuildMessage(uint8_t *root, const MqMode mode,
                                const uint8_t *data, const std::size_t root_len,
                                const std::size_t data_len) {
  FORMULA_GUARD(root_len != data_len + 3, false,
                "Not matched the message components!");

  root[0] = static_cast<uint8_t>(MqMode::MQ_START);
  root[1] = static_cast<uint8_t>(mode);
  root[root_len - 1] = static_cast<uint8_t>(MqMode::MQ_STOP);
  memcpy(&root[2], data, root_len - 3);
  return CheckMessage(mode, root);
}

bool MessageQueue::CheckMessage(MqMode mode, const uint8_t *msg) {
  FORMULA_GUARD(MqMode::MQ_START != static_cast<MqMode>(msg[0]), false,
                "Error on the start protocal in the message.");

  FORMULA_GUARD(MqMode::MQ_STOP != static_cast<MqMode>(msg[len - 1]), false,
                "Error on the end protocol in the message.");

  FORMULA_GUARD(mode != static_cast<MqMode>(msg[1]), false,
                "Error on the mode protocol in the message.");

  return true;
}

mqd_t MessageQueue::GetCurFd(void) const { return this->fd; }
const char *MessageQueue::GetMqPath(void) const { return this->mq_path; }

int MessageQueue::Open(const char *mq_path, const std::size_t len) {
  struct mq_attr attr = {
      .mq_flags = 0, .mq_maxmsg = 2, .mq_msgsize = static_cast<int64_t>(len)};
  fd = mq_open(mq_path, O_RDWR, 0666, &attr);
  FORMULA_GUARD(fd == static_cast<mqd_t>(-1), -EPERM,
                "Failed to open the message queue.");

  return 0;
}

int MessageQueue::Close(void) {
  if (fd > 0)
    mq_close(fd);

  fd = static_cast<mqd_t>(-1);
  len = 0;

  return 0;
}
