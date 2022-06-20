/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef THREAD_H
#define THREAD_H

#include <atomic>
#include <memory>
#include <thread>

class Thread {
public: // NOLINT
  Thread();
  virtual ~Thread() = default;

  virtual bool Start(void *arg);
  virtual bool Stop(void);

  virtual void *Run(void *arg) = 0;

protected: // NOLINT
  std::atomic<bool> status;

private: // NOLINT
  std::thread th;

  void Main(void *arg);
};

#endif /* THREAD_H */
