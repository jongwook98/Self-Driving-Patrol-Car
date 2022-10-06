/* vim: set tabstop=4:softtabstop=4:shiftwidth=4:ffs=unix */
#ifndef SDPC_MESSAGE_QUEUE_H
#define SDPC_MESSAGE_QUEUE_H

#include <sdpc/common/sdpc_common.h>

typedef enum message_queue_mode
{
    MQ_START = 0x55,
    MQ_STOP = 0x77,
    VEHICLE_MODE = 0,
    LANE_MODE,
    TRAFFIC_MODE,
    LIDAR_MODE,
    DEEPLEARNING_MODE,
    TOTAL_MQ
} mq_mode_e;

static inline int check_message(const mq_mode_e mode, const char *msg)
{
    size_t len = strlen(msg);
    FORMULA_GUARD(len == 0, -EPERM, "Empty message,,");

    int ret =
        (msg[0] != MQ_START || msg[1] != (char)mode || msg[len - 1] != MQ_STOP);
    FORMULA_GUARD(ret, -EPERM, "Message components error.");

    return 0;
}

int sdpc_message_queue_init(void);
int sdpc_message_queue_exit(void);

int sdpc_message_queue_send(const mq_mode_e mode, const char *send_msg,
                            const size_t len);
int sdpc_message_queue_receive(const mq_mode_e mode, char *read_buf,
                               const size_t len);

#endif /* SDPC_MESSAGE_QUEUE_H */
