#include <sdpc/common/sdpc_common.h>
#include <sdpc/common/sdpc_msgque.h>
#include <sdpc/common/sdpc_uart.h>
#include <sdpc/sensor_fusion/sdpc_sensor_fusion.h>

struct sensor_fusion_data
{
    pthread_t th;
    int escape;

    sync_t *cur_sync;
    RT_MODEL_sf_simulink_T *sf_simulink_M;
};

static struct sensor_fusion_data *_sensor_fusion_create(void);
static int _sensor_fusion_destroy(struct sensor_fusion_data *del);

static inline int _sensor_fusion_create_thread(struct sensor_fusion_data *sf);
static inline void _sensor_fusion_destroy_thread(struct sensor_fusion_data *sf);

static struct sensor_fusion_data *g_sensor_fusion;

int sdpc_sensor_fusion_init(void)
{
    FORMULA_GUARD(g_sensor_fusion, -EPERM, ERR_INVALID_PTR);

    g_sensor_fusion = _sensor_fusion_create();
    FORMULA_GUARD(g_sensor_fusion == NULL, -EPERM, ERR_INVALID_PTR);

    return 0;
}

int sdpc_sensor_fusion_exit(void)
{
    FORMULA_GUARD(g_sensor_fusion == NULL, -EPERM, ERR_INVALID_PTR);

    _sensor_fusion_destroy(g_sensor_fusion);
    g_sensor_fusion = NULL;

    return 0;
}

static struct sensor_fusion_data *_sensor_fusion_create(void)
{
    struct sensor_fusion_data *new = malloc(sizeof(struct sensor_fusion_data));
    FORMULA_GUARD(new == NULL, NULL, ERR_MEMORY_SHORTAGE);

    new->sf_simulink_M = sf_simulink();
    FORMULA_GUARD_WITH_EXIT_FUNC(new->sf_simulink_M == NULL, NULL,
                                 _sensor_fusion_destroy(new), "");

    /* Initialize model */
    sf_simulink_initialize(new->sf_simulink_M);

    new->escape = ON;

    new->cur_sync = sync_create();
    FORMULA_GUARD_WITH_EXIT_FUNC(new->cur_sync == NULL, NULL,
                                 _sensor_fusion_destroy(new), "");

    int ret = _sensor_fusion_create_thread(new);
    FORMULA_GUARD_WITH_EXIT_FUNC(ret < 0, NULL, _sensor_fusion_destroy(new),
                                 "");

    return new;
}

static int _sensor_fusion_destroy(struct sensor_fusion_data *del)
{
    FORMULA_GUARD(del == NULL, -EPERM, ERR_INVALID_PTR);

    del->escape = OFF;

    _sensor_fusion_destroy_thread(del);

    sf_simulink_terminate(del->sf_simulink_M);

    sync_destroy(&del->cur_sync);
    PTR_FREE(del);

    return 0;
}

static void *_do_thread(void *arg)
{
    struct sensor_fusion_data *sf = (struct sensor_fusion_data *)arg;
    sync_t *cur_sync = sf->cur_sync;
    RT_MODEL_sf_simulink_T *model = sf->sf_simulink_M;

    send_signal(cur_sync);

    while (sf->escape)
    {
        /* TODO */
        sf_simulink_step(model);
    }

    return NULL;
}

static inline int _sensor_fusion_create_thread(struct sensor_fusion_data *sf)
{
    sync_t *sync = sf->cur_sync;

    pthread_mutex_lock(&sync->lock);
    int ret = pthread_create(&sf->th, NULL, _do_thread, sf);
    FORMULA_GUARD_WITH_EXIT_FUNC(ret < 0, -EPERM,
                                 pthread_mutex_unlock(&sync->lock),
                                 "Failed to create the pthread.");
    pthread_cond_wait(&sync->cv, &sync->lock);
    pthread_mutex_unlock(&sync->lock);

    return 0;
}

static inline void _sensor_fusion_destroy_thread(struct sensor_fusion_data *sf)
{
    pthread_join(sf->th, NULL);
}
