#include <sdpc/common/sdpc_common.h>
#include <sdpc/lane_recognition/sdpc_lane.h>

struct lane_data
{
    pthread_t th;
    int escape;

    sync_t *cur_sync;
};

static struct lane_data *_lane_create(void);
static int _lane_destroy(struct lane_data *del);

static inline int _lane_create_thread(struct lane_data *lane);
static inline void _lane_destroy_thread(struct lane_data *lane);

static struct lane_data *g_lane;

int sdpc_lane_init(void)
{
    FORMULA_GUARD(g_lane, -EPERM, ERR_INVALID_PTR);

    g_lane = _lane_create();
    FORMULA_GUARD(g_lane == NULL, -EPERM, ERR_INVALID_PTR);

    return 0;
}

int sdpc_lane_exit(void)
{
    FORMULA_GUARD(g_lane == NULL, -EPERM, ERR_INVALID_PTR);

    _lane_destroy(g_lane);
    g_lane = NULL;

    return 0;
}

static struct lane_data *_lane_create(void)
{
    struct lane_data *new = malloc(sizeof(struct lane_data));
    FORMULA_GUARD(new == NULL, NULL, ERR_MEMORY_SHORTAGE);

    new->escape = ON;

    new->cur_sync = sync_create();
    FORMULA_GUARD_WITH_EXIT_FUNC(new->cur_sync == NULL, NULL,
                                 _lane_destroy(new), "");

    int ret = _lane_create_thread(new);
    FORMULA_GUARD_WITH_EXIT_FUNC(ret < 0, NULL, _lane_destroy(new), "");

    return new;
}

static int _lane_destroy(struct lane_data *del)
{
    FORMULA_GUARD(del == NULL, -EPERM, ERR_INVALID_PTR);

    del->escape = OFF;
    _lane_destroy_thread(del);
    sync_destroy(&del->cur_sync);
    PTR_FREE(del);

    return 0;
}

static void *_do_thread(void *arg)
{
    struct lane_data *lane = (struct lane_data *)arg;
    sync_t *cur_sync = lane->cur_sync;

    pthread_mutex_lock(&cur_sync->lock);
    pthread_cond_signal(&cur_sync->cv);
    pthread_mutex_unlock(&cur_sync->lock);

    while (lane->escape)
    {
        /* TODO */
    }

    return NULL;
}

static inline int _lane_create_thread(struct lane_data *lane)
{
    sync_t *sync = lane->cur_sync;

    pthread_mutex_lock(&sync->lock);
    int ret = pthread_create(&lane->th, NULL, _do_thread, lane);
    FORMULA_GUARD_WITH_EXIT_FUNC(ret < 0, -EPERM,
                                 pthread_mutex_unlock(&sync->lock),
                                 "Failed to create the pthread.");
    pthread_cond_wait(&sync->cv, &sync->lock);
    pthread_mutex_unlock(&sync->lock);

    return 0;
}

static inline void _lane_destroy_thread(struct lane_data *lane)
{
    pthread_join(lane->th, NULL);
}
