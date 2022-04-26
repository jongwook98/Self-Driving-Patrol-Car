/* vim: set tabstop=4:softtabstop=4:shiftwidth=4:ffs=unix */
#ifndef SDPC_COMMON_H
#define SDPC_COMMON_H

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_BUF 128

#define ERR_INVALID_PTR "Invalid Pointer."
#define ERR_INVALID_RET "Invalild return value."
#define ERR_INVALID_FD "Invalid File Descripter."
#define ERR_INVALID_PARAMS "Invalid Parameters."
#define ERR_MEMORY_SHORTAGE "Failed to allocate the memory."

#define unlikely(x) __builtin_expect(!!(x), 0)

#define ERR_MSG(fmt, ...)                                                      \
    do                                                                         \
    {                                                                          \
        fprintf(stderr, "[err] " fmt " / func : %s, line : %d\n",              \
                ##__VA_ARGS__, __func__, __LINE__);                            \
    } while (0)

#define FORMULA_GUARD(formula, ret, msg, ...)                                  \
    do                                                                         \
    {                                                                          \
        if (unlikely(formula))                                                 \
        {                                                                      \
            if (strcmp(msg, ""))                                               \
                ERR_MSG(msg, ##__VA_ARGS__);                                   \
            return ret;                                                        \
        }                                                                      \
    } while (0)

#define FORMULA_GUARD_WITH_EXIT_FUNC(formula, ret, func, msg, ...)             \
    do                                                                         \
    {                                                                          \
        if (unlikely(formula))                                                 \
        {                                                                      \
            if (strcmp(msg, ""))                                               \
                ERR_MSG(msg, ##__VA_ARGS__);                                   \
            return func, ret;                                                  \
        }                                                                      \
    } while (0)

#define PTR_FREE(ptr)                                                          \
    do                                                                         \
    {                                                                          \
        if (ptr)                                                               \
        {                                                                      \
            free(ptr);                                                         \
            ptr = NULL;                                                        \
        }                                                                      \
    } while (0)

enum
{
    OFF,
    ON
};

typedef struct sync sync_t;

struct sync
{
    pthread_cond_t cv;
    pthread_mutex_t lock;
};

static inline int check_permission(int (*pfunc_exit)(void))
{
    if (getuid() == 0 || geteuid() == 0)
        return 0;

    ERR_MSG("User is not root!");
    if (pfunc_exit && *pfunc_exit)
        (*pfunc_exit)();

    return -EPERM;
}

static inline void wait_signal(sync_t *sync)
{
    pthread_mutex_lock(&sync->lock);
    pthread_cond_wait(&sync->cv, &sync->lock);
    pthread_mutex_unlock(&sync->lock);
}

static inline void send_signal(sync_t *sync)
{
    pthread_mutex_lock(&sync->lock);
    pthread_cond_signal(&sync->cv);
    pthread_mutex_unlock(&sync->lock);
}

ssize_t readn(int fd, void *ptr, size_t n);
ssize_t writen(int fd, const void *ptr, size_t n);

sync_t *sync_create(void);
int sync_destroy(sync_t **const sync);

#endif /* SDPC_COMMON_H */
