/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#include <lane_detection/common/thread.h>

Thread::Thread() : th(), status(false) {}

bool Thread::Start(void *arg) {
  status = true;
  th = std::thread(&Thread::Main, this, arg);
  return true;
}

bool Thread::Stop(void) {
  status = false;

  if (th.joinable() == false)
    return false;

  th.join();
  return true;
}

void Thread::Main(void *arg) { Run(arg); }
