/* Copyright 2022. Park, Sangjae/Bae, Youwon all rights reserved */

#ifndef COMMON_H
#define COMMON_H

#include <glog/logging.h>

#include <iostream>

#define ERR_INVALID_PTR "Invalid Pointer."
#define ERR_INVALID_RET "Invalild return value."
#define ERR_INVALID_FD "Invalid File Descripter."
#define ERR_INVALID_PARAMS "Invalid Parameters."
#define ERR_MEMORY_SHORTAGE "Failed to allocate the memory."

#define MAX_BUF 128

#define unlikely(x) __builtin_expect(!!(x), 0)

#define FORMULA_GUARD(formula, ret, msg, ...)                                  \
  do {                                                                         \
    if (unlikely(formula)) {                                                   \
      if (msg != "")                                                           \
        ERR_MSG(msg, ##__VA_ARGS__);                                           \
      return ret;                                                              \
    }                                                                          \
  } while (0)

#define FORMULA_GUARD_WITH_EXIT_FUNC(formula, ret, func, msg, ...)             \
  do {                                                                         \
    if (unlikely(formula)) {                                                   \
      if (msg != "")                                                           \
        ERR_MSG(msg, ##__VA_ARGS__);                                           \
      return func, ret;                                                        \
    }                                                                          \
  } while (0)

#define EXIT_GUARD(formula, msg, ...)                                          \
  do {                                                                         \
    if (unlikely(formula)) {                                                   \
      if (msg != "")                                                           \
        EXIT_MSG(msg, ##__VA_ARGS__);                                          \
    }                                                                          \
  } while (0)

template <typename... Args> static inline void DEBUG_MSG(Args &&... args) {
  (LOG(INFO) << ... << args);
}

template <typename... Args> static inline void WARNING_MSG(Args &&... args) {
  (LOG(WARNING) << ... << args);
}

template <typename... Args> static inline void ERR_MSG(Args &&... args) {
  (LOG(ERROR) << ... << args);
}

template <typename... Args> static inline void EXIT_MSG(Args &&... args) {
  (LOG(FATAL) << ... << args);
}

#endif /* COMMON_H */
