#include <sdpc/common/sdpc_common.h>
#include <sdpc/common/sdpc_msgque.h>
#include <sdpc/sensor_fusion/sdpc_sensor_fusion.h>
#include <sdpc/vehicle_recognition/sdpc_vehicle.h>

#define VEHICLE_THREAD_NUM 3

#define LIDAR_DONE 0x1
#define DEEPLEARNING_DONE 0x2

struct vehicle_data
{
    pthread_t th[VEHICLE_THREAD_NUM];
    int escape;

    sync_t *main_sync;
    sync_t *lidar_sync;
    sync_t *deep_sync;
};

static struct vehicle_data *_vehicle_create(void);
static int _vehicle_destroy(struct vehicle_data *del);

static void *_main_thread(void *arg);
static void *_lidar_thread(void *arg);
static void *_deeplearning_thread(void *arg);

static inline int _vehicle_create_thread(struct vehicle_data *vehicle);
static inline void _vehicle_destroy_thread(struct vehicle_data *vehicle);

static struct vehicle_data *g_vehicle;

int sdpc_vehicle_init(void)
{
    FORMULA_GUARD(g_vehicle, -EPERM, ERR_INVALID_PTR);

    g_vehicle = _vehicle_create();
    FORMULA_GUARD(g_vehicle == NULL, -EPERM, ERR_INVALID_PTR);

    return 0;
}

int sdpc_vehicle_exit(void)
{
    FORMULA_GUARD(g_vehicle == NULL, -EPERM, ERR_INVALID_PTR);

    _vehicle_destroy(g_vehicle);
    g_vehicle = NULL;

    return 0;
}

static struct vehicle_data *_vehicle_create(void)
{
    struct vehicle_data *new = malloc(sizeof(struct vehicle_data));
    FORMULA_GUARD(new == NULL, NULL, ERR_MEMORY_SHORTAGE);

    new->escape = ON;

    new->main_sync = sync_create();
    new->lidar_sync = sync_create();
    new->deep_sync = sync_create();
    FORMULA_GUARD_WITH_EXIT_FUNC(new->main_sync == NULL ||
                                     new->lidar_sync == NULL ||
                                     new->deep_sync == NULL,
                                 NULL, _vehicle_destroy(new), "");

    int ret = _vehicle_create_thread(new);
    FORMULA_GUARD_WITH_EXIT_FUNC(ret < 0, NULL, _vehicle_destroy(new), "");

    return new;
}

static int _vehicle_destroy(struct vehicle_data *del)
{
    FORMULA_GUARD(del == NULL, -EPERM, ERR_INVALID_PTR);

    del->escape = OFF;

    _vehicle_destroy_thread(del);
    sync_destroy(&del->deep_sync);
    sync_destroy(&del->lidar_sync);
    sync_destroy(&del->main_sync);
    PTR_FREE(del);

    return 0;
}

static void *_main_thread(void *arg)
{
    struct vehicle_data *vehicle = (struct vehicle_data *)arg;
    sync_t *main_sync = vehicle->main_sync;
    sync_t *lidar_sync = vehicle->lidar_sync;
    sync_t *deep_sync = vehicle->deep_sync;

    char read_buf[4] = {
        0,
    };

    /* send */
    send_signal(main_sync);

    while (vehicle->escape)
    {
        /* broadcast */
        send_signal(lidar_sync);
        send_signal(deep_sync);

        sdpc_message_queue_receive(VEHICLE_MODE, read_buf, sizeof(read_buf));
        memset(read_buf, 0x0, sizeof(read_buf));
    }

    return NULL;
}

static void *_lidar_thread(void *arg)
{
    struct vehicle_data *vehicle = (struct vehicle_data *)arg;
    sync_t *main_sync = vehicle->main_sync;
    sync_t *lidar_sync = vehicle->lidar_sync;
    char send_data[] = {MQ_START, VEHICLE_MODE, LIDAR_DONE, MQ_STOP};

    char read_buf[5] = {
        0,
    };

    /* send */
    send_signal(main_sync);

    while (vehicle->escape)
    {
        /* wait */
        wait_signal(lidar_sync);

        /* TODO */
        sdpc_message_queue_receive(LIDAR_MODE, read_buf, sizeof(read_buf));

        /* send to use the message queue */
        sdpc_message_queue_send(VEHICLE_MODE, send_data, sizeof(send_data));
    }

    return NULL;
}

static void *_deeplearning_thread(void *arg)
{
    struct vehicle_data *vehicle = (struct vehicle_data *)arg;
    sync_t *main_sync = vehicle->main_sync;
    sync_t *deep_sync = vehicle->deep_sync;
    char send_data[] = {MQ_START, VEHICLE_MODE, DEEPLEARNING_DONE, MQ_STOP};

    /* send */
    send_signal(main_sync);

    while (vehicle->escape)
    {
        /* wait */
        wait_signal(deep_sync);

        /* TODO */

        /* send to use the message queue */
        sdpc_message_queue_send(VEHICLE_MODE, send_data, sizeof(send_data));
    }

    return NULL;
}

static inline int _vehicle_create_thread(struct vehicle_data *vehicle)
{
    sync_t *sync = vehicle->main_sync;
    void *(*pfunc[VEHICLE_THREAD_NUM])(void *arg) = {
        _main_thread, _lidar_thread, _deeplearning_thread};

    for (int i = 0; i < VEHICLE_THREAD_NUM; i++)
    {
        pthread_mutex_lock(&sync->lock);
        int ret = pthread_create(&vehicle->th[i], NULL, pfunc[i], vehicle);
        FORMULA_GUARD_WITH_EXIT_FUNC(ret < 0, -EPERM,
                                     pthread_mutex_unlock(&sync->lock),
                                     "Failed to create the pthread.");
        pthread_cond_wait(&sync->cv, &sync->lock);
        pthread_mutex_unlock(&sync->lock);
    }

    return 0;
}

static inline void _vehicle_destroy_thread(struct vehicle_data *vehicle)
{
    /* waiting the tread sequence */
    usleep(1);

    /* sending the signal for the canceling */
    send_signal(vehicle->lidar_sync);
    send_signal(vehicle->deep_sync);

    for (int i = 0; i < VEHICLE_THREAD_NUM; i++)
        pthread_join(vehicle->th[i], NULL);
}
