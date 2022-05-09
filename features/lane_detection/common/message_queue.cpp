/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/common/message_queue.h>

#include <fcntl.h>
#include <sys/stat.h>

#include <cstring>

MessageQueue::MessageQueue(const char *mq_path, const std::size_t len)
    : fd(static_cast<mqd_t>(-1)), len(len), mq_path(mq_path) {
  int ret = Open(mq_path, len);
  if (ret < 0)
    exit(-EPERM);
}

MessageQueue::~MessageQueue() { Close(); }

int MessageQueue::Send(const char *send_msg, const std::size_t len) {
  int ret = mq_send(fd, send_msg, len, 0);
  if (ret < 0) {
    std::cout << "Failed to send the message." << std::endl;
    return -EPERM;
  }

  return ret;
}

int MessageQueue::Send(const char *send_msg, const std::size_t len, int sec) {
  struct timespec limit;
  clock_gettime(CLOCK_REALTIME, &limit);
  limit.tv_sec += sec;

  int ret = mq_timedsend(fd, send_msg, len, 0, &limit);
  if (ret < 0) {
    std::cout << "Failed to send the message." << std::endl;
    return -EPERM;
  }

  return ret;
}

int MessageQueue::Receive(char *read_buf, const std::size_t len) {
  ssize_t ret = mq_receive(fd, read_buf, len, NULL);
  if (ret < 0) {
    std::cout << "Failed to receive the message." << std::endl;
    return -EPERM;
  }

  return static_cast<int>(ret);
}

int MessageQueue::Receive(char *read_buf, const std::size_t len, int sec) {
  struct timespec limit;
  clock_gettime(CLOCK_REALTIME, &limit);
  limit.tv_sec += sec;

  ssize_t ret = mq_timedreceive(fd, read_buf, len, NULL, &limit);
  if (ret < 0) {
    std::cout << "Failed to receive the message." << std::endl;
    return -EPERM;
  }

  return static_cast<int>(ret);
}

bool MessageQueue::BuildMessage(uint8_t *root, const MqMode mode,
                                const uint8_t *data,
                                const std::size_t root_len,
                                const std::size_t data_len) {
  if (root_len != data_len + 3) {
    std::cout << "Not matched the message components!" << std::endl;
    return false;
  }

  root[0] = static_cast<uint8_t>(MqMode::MQ_START);
  root[1] = static_cast<uint8_t>(mode);
  root[root_len - 1] = static_cast<uint8_t>(MqMode::MQ_STOP);
  memcpy(&root[2], data, root_len - 3);
  return CheckMessage(mode, root);
}

bool MessageQueue::CheckMessage(MqMode mode, const uint8_t *msg) {
  if (MqMode::MQ_START != static_cast<MqMode>(msg[0])) {
    std::cout << "Error on the start protocol in the message." << std::endl;
    return false;
  }

  if (MqMode::MQ_STOP != static_cast<MqMode>(msg[len - 1])) {
    std::cout << "Error on the end protocol in the message." << std::endl;
    return false;
  }

  if (mode != static_cast<MqMode>(msg[1])) {
    std::cout << "Error on the mode protocol in the message." << std::endl;
    return false;
  }

  return true;
}

mqd_t MessageQueue::GetCurFd(void) const { return fd; }
const char *MessageQueue::GetMqPath(void) const { return mq_path; }

int MessageQueue::Open(const char *mq_path, const std::size_t len) {
  struct mq_attr attr = {
      .mq_flags = 0, .mq_maxmsg = 2, .mq_msgsize = static_cast<int64_t>(len)};
  fd = mq_open(mq_path, O_RDWR, 0666, &attr);
  if (fd == static_cast<mqd_t>(-1)) {
    std::cout << "Failed to open the message queue." << std::endl;
    return -EPERM;
  }

  return 0;
}

int MessageQueue::Close() {
  if (fd > 0)
    mq_close(fd);

  fd = static_cast<mqd_t>(-1);
  len = 0;

  return 0;
}
