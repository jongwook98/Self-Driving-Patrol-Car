#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>

#include <sdpc/common/sdpc_common.h>
#include <sdpc/common/sdpc_msgque.h>

struct sdpc_message_queue
{
    mqd_t fd;
    size_t len;
    char path[MAX_BUF];
};

static struct sdpc_message_queue *_message_queue_create(const char *path,
                                                        const size_t len);
static int _message_queue_destroy(struct sdpc_message_queue *del);

static int _message_queue_open(struct sdpc_message_queue *mq);
static int _message_queue_close(struct sdpc_message_queue *mq);

static struct sdpc_message_queue *g_msgque[TOTAL_MQ];

int sdpc_message_queue_init(void)
{
    int i;
    for (i = 0; i < TOTAL_MQ; i++)
        FORMULA_GUARD(g_msgque[i], -EPERM, ERR_INVALID_PTR);

    const char *mq_path[] = {"/core_vehicle", "/lane_detection_to_core",
                             "/lidar_data_to_core"};
    const long mq_msg_size[] = {sizeof(char) * 4, sizeof(char) * 5,
                                sizeof(char) * 5};

    for (i = 0; i < TOTAL_MQ; i++)
    {
        g_msgque[i] = _message_queue_create(mq_path[i], mq_msg_size[i]);
        FORMULA_GUARD_WITH_EXIT_FUNC(g_msgque[i] == NULL, -EPERM,
                                     sdpc_message_queue_exit(), "");
    }

    return 0;
}

int sdpc_message_queue_exit(void)
{
    for (int i = TOTAL_MQ - 1; i >= 0; i--)
    {
        if (g_msgque[i] == NULL)
            continue;

        _message_queue_destroy(g_msgque[i]);
        g_msgque[i] = NULL;
    }

    return 0;
}

int sdpc_message_queue_send(const mq_mode_e mode, const char *send_msg,
                            const size_t len)
{
    FORMULA_GUARD(mode >= TOTAL_MQ || send_msg == NULL, -EPERM,
                  ERR_INVALID_PARAMS);

    struct timespec limit;
    clock_gettime(CLOCK_REALTIME, &limit);
    limit.tv_sec += 1;

    /* set the timeout 1sec */
    int ret = mq_timedsend(g_msgque[mode]->fd, send_msg, len, 0, &limit);
    FORMULA_GUARD(ret < 0, -EPERM, "Failed to send the message.");

    return ret;
}

int sdpc_message_queue_receive(const mq_mode_e mode, char *read_buf,
                               const size_t len)
{
    FORMULA_GUARD(mode >= TOTAL_MQ || read_buf == NULL, -EPERM,
                  ERR_INVALID_PARAMS);

    struct timespec limit;
    clock_gettime(CLOCK_REALTIME, &limit);
    limit.tv_sec += 1;

    /* set the timeout 1sec */
    int ret = mq_timedreceive(g_msgque[mode]->fd, read_buf, len, NULL, &limit);
    FORMULA_GUARD_WITH_EXIT_FUNC(ret < 0, -EPERM, memset(read_buf, 0x0, len),
                                 "Failed to receive the message.");

    return ret;
}

static struct sdpc_message_queue *_message_queue_create(const char *path,
                                                        const size_t len)
{
    struct sdpc_message_queue *new = malloc(sizeof(struct sdpc_message_queue));
    FORMULA_GUARD(new == NULL, NULL, ERR_MEMORY_SHORTAGE);

    new->fd = (mqd_t)-1;
    new->len = len;
    snprintf(new->path, sizeof(new->path), "%s", path);

    int ret = _message_queue_open(new);
    FORMULA_GUARD_WITH_EXIT_FUNC(ret < 0, NULL, _message_queue_close(new), "");

    return new;
}

static int _message_queue_destroy(struct sdpc_message_queue *del)
{
    FORMULA_GUARD(del == NULL, -EPERM, ERR_INVALID_PTR);

    _message_queue_close(del);

    memset(del->path, 0x0, sizeof(del->path));
    del->len = 0;
    del->fd = (mqd_t)-1;

    PTR_FREE(del);

    return 0;
}

static int _message_queue_open(struct sdpc_message_queue *mq)
{
    FORMULA_GUARD(mq->path == NULL, -EPERM, ERR_INVALID_PTR);

    struct mq_attr attr = {.mq_flags = 0,
                           .mq_maxmsg = 2,
                           .mq_msgsize = (long)mq->len,
                           .mq_curmsgs = 0};

    mq->fd = mq_open(mq->path, O_RDWR | O_CREAT | O_EXCL, 0660, &attr);
    FORMULA_GUARD(mq->fd == (mqd_t)-1, -EPERM,
                  "Failed to open the message queue %s", mq->path);

    return 0;
}

static int _message_queue_close(struct sdpc_message_queue *mq)
{
    if (mq->fd > 0)
    {
        mq_close(mq->fd);
        mq_unlink(mq->path);
    }

    return 0;
}
