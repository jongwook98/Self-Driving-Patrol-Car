#include <sdpc/common/sdpc_common.h>
#include <sdpc/common/sdpc_msgque.h>
#include <sdpc/sensor_fusion/sdpc_sensor_fusion.h>
#include <sdpc/vehicle_recognition/sdpc_vehicle.h>

#define VEHICLE_THREAD_NUM 5

#define LIDAR_DONE 0x1
#define DEEPLEARNING_DONE 0x2
#define LANE_DONE 0x3
#define TRAFFIC_DONE 0x4

struct vehicle_data
{
    pthread_t th[VEHICLE_THREAD_NUM];
    int escape;

    RT_MODEL_sf_simulink_T *sf_model;

    sync_t *main_sync;
    sync_t *lidar_sync;
    sync_t *deep_sync;
    sync_t *lane_sync;
    sync_t *traffic_sync;
};

static struct vehicle_data *_vehicle_create(void);
static int _vehicle_destroy(struct vehicle_data *del);

static void *_main_thread(void *arg);
static void *_lidar_thread(void *arg);
static void *_deeplearning_thread(void *arg);
static void *_lane_thread(void *arg);
static void *_traffic_thread(void *arg);

static inline int _vehicle_create_thread(struct vehicle_data *vehicle);
static inline void _vehicle_destroy_thread(struct vehicle_data *vehicle);

static inline void _data_convert(void *new, const char *old, int num, size_t size);

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

    new->sf_model = sensor_fusion_create();

    new->main_sync = sync_create();
    new->lidar_sync = sync_create();
    new->deep_sync = sync_create();
    new->lane_sync = sync_create();
    new->traffic_sync = sync_create();

    FORMULA_GUARD_WITH_EXIT_FUNC(new->sf_model == NULL ||
                                     new->main_sync == NULL ||
                                     new->lidar_sync == NULL ||
                                     new->deep_sync == NULL ||
                                     new->lane_sync == NULL ||
                                     new->traffic_sync == NULL,
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

    sync_destroy(&del->traffic_sync);
    sync_destroy(&del->lane_sync);
    sync_destroy(&del->deep_sync);
    sync_destroy(&del->lidar_sync);
    sync_destroy(&del->main_sync);

    sensor_fusion_destroy(&del->sf_model);
    PTR_FREE(del);

    return 0;
}

static void *_main_thread(void *arg)
{
    struct vehicle_data *vehicle = (struct vehicle_data *)arg;
    sync_t *main_sync = vehicle->main_sync;
    sync_t *lidar_sync = vehicle->lidar_sync;
    sync_t *deep_sync = vehicle->deep_sync;
    sync_t *lane_sync = vehicle->lane_sync;
    sync_t *traffic_sync = vehicle->traffic_sync;

    RT_MODEL_sf_simulink_T *sf_main = vehicle->sf_model;
    struct timeval rt;
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
        send_signal(lane_sync);
        send_signal(traffic_sync);

        /* TODO */
        sdpc_message_queue_receive(VEHICLE_MODE, read_buf, sizeof(read_buf));

        /* execute sensor fusion function */
        gettimeofday(&rt, NULL);
        sf_main->inputs->Input2.time = rt.tv_usec;
        sf_simulink_step(sf_main);

        memset(read_buf, 0x0, sizeof(read_buf));
    }

    return NULL;
}

static void *_lidar_thread(void *arg)
{
    struct vehicle_data *vehicle = (struct vehicle_data *)arg;
    sync_t *main_sync = vehicle->main_sync;
    sync_t *lidar_sync = vehicle->lidar_sync;
    ExtU_sf_simulink_T *sf_lidar = vehicle->sf_model->inputs;

    char send_data[] = {MQ_START, VEHICLE_MODE, LIDAR_DONE, MQ_STOP};
    char read_buf[45] = {
        0,
    };
    uint16_T recv[20] = {
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
        _data_convert(recv, read_buf, 20, sizeof(uint16_T));

        /* sensor fusion data matching */
        sf_lidar->Input1.detected_number = read_buf[3];
        memcpy(sf_lidar->Input1.angle, recv, sizeof(uint16_T) * 10);
        memcpy(sf_lidar->Input1.dist, &recv[10], sizeof(uint16_T) * 10);

        memset(read_buf, 0x0, sizeof(read_buf));
        memset(recv, 0x0, sizeof(recv));

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
    ExtU_sf_simulink_T *sf_deepl = vehicle->sf_model->inputs;

    char send_data[] = {MQ_START, VEHICLE_MODE, DEEPLEARNING_DONE, MQ_STOP};
    char read_buf[26] = {
        0,
    };
    uint16_T car_angle[10] = {
        0,
    };

    /* send */
    send_signal(main_sync);

    while (vehicle->escape)
    {
        /* wait */
        wait_signal(deep_sync);

        /* TODO */
        sdpc_message_queue_receive(DEEPLEARNING_MODE, read_buf, sizeof(read_buf));

        /* sensor fusion data matching */
        sf_deepl->Input.car_check_flag = read_buf[3];
        memcpy(sf_deepl->Input.object_id, &read_buf[4], sizeof(uint8_T) * 10);
        for(int i = 0; i < 10; i++)
        {
            if(read_buf[i+14] >= 90)
                car_angle[i] = read_buf[i+14] - 90;
            else
                car_angle[i] = read_buf[i+14] + 270;
        }
        memcpy(sf_deepl->Input.object_angle, car_angle, sizeof(uint16_T) * 10);
        sf_deepl->Input.crack_flag = read_buf[24];

        memset(read_buf, 0x0, sizeof(read_buf));
        memset(car_angle, 0x0, sizeof(uint16_T) * 10);

        /* send to use the message queue */
        sdpc_message_queue_send(VEHICLE_MODE, send_data, sizeof(send_data));
    }

    return NULL;
}

static void *_lane_thread(void *arg)
{
    struct vehicle_data *vehicle = (struct vehicle_data *)arg;
    sync_t *main_sync = vehicle->main_sync;
    sync_t *lane_sync = vehicle->lane_sync;
    ExtU_sf_simulink_T *sf_lane = vehicle->sf_model->inputs;

    char send_data[] = {MQ_START, VEHICLE_MODE, LANE_DONE, MQ_STOP};
    char read_buf[14] = {
        0
    };
    int64_t stop_inf;

    /* send */
    send_signal(main_sync);

    while (vehicle->escape)
    {
        /* wait */
        wait_signal(lane_sync);

        /* TODO */
        sdpc_message_queue_receive(LANE_MODE, read_buf, sizeof(read_buf));
        _data_convert(&stop_inf, read_buf, 1, sizeof(int64_t));

        /* sensor fusion data matching */
        sf_lane->Input.stop_line_dist = (double)(stop_inf / 100000);
        sf_lane->Input.line_angle = read_buf[3];
        sf_lane->Input.change_lane_dir = read_buf[12];

        memset(read_buf, 0x0, sizeof(read_buf));
        memset(&stop_inf, 0x0, sizeof(int64_t));

        /* send to use the message queue */
        sdpc_message_queue_send(VEHICLE_MODE, send_data, sizeof(send_data));
    }

    return NULL;
}

static void *_traffic_thread(void *arg)
{
    struct vehicle_data *vehicle = (struct vehicle_data *)arg;
    sync_t *main_sync = vehicle->main_sync;
    sync_t *traffic_sync = vehicle->traffic_sync;
    ExtU_sf_simulink_T *sf_traffic = vehicle->sf_model->inputs;

    char send_data[] = {MQ_START, VEHICLE_MODE, TRAFFIC_DONE, MQ_STOP};
    char read_buf[5] = {
        0,
    };

    /* send */
    send_signal(main_sync);

    while (vehicle->escape)
    {
        /* wait */
        wait_signal(traffic_sync);

        /* TODO */
        sdpc_message_queue_receive(TRAFFIC_MODE, read_buf, sizeof(read_buf));

        /* sensor fusion data matching */
        sf_traffic->Input.sig_flag = read_buf[3];

        memset(read_buf, 0x0, sizeof(read_buf));

        /* send to use the message queue */
        sdpc_message_queue_send(VEHICLE_MODE, send_data, sizeof(send_data));
    }

    return NULL;
}

static inline int _vehicle_create_thread(struct vehicle_data *vehicle)
{
    sync_t *sync = vehicle->main_sync;
    void *(*pfunc[VEHICLE_THREAD_NUM])(void *arg) = {
        _main_thread, _lidar_thread, _deeplearning_thread, _lane_thread,
        _traffic_thread};

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
    send_signal(vehicle->lane_sync);
    send_signal(vehicle->traffic_sync);

    for (int i = 0; i < VEHICLE_THREAD_NUM; i++)
        pthread_join(vehicle->th[i], NULL);
}

static inline void _data_convert(void *new, const char *old, int num, size_t size)
{
    for (int i = 0; i < num; i++)
        memcpy(new + i*size, old + 4 + i*size, size);
}
