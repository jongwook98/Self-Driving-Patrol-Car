#include <sdpc/common/sdpc_common.h>
#include <sdpc/sensor_fusion/sdpc_sensor_fusion.h>

static RT_MODEL_sf_simulink_T *_sensor_fusion_create(void);
static int _sensor_fusion_destroy(RT_MODEL_sf_simulink_T *del);

static RT_MODEL_sf_simulink_T *g_sf_model;

int sdpc_sensor_fusion_init(void)
{
    FORMULA_GUARD(g_sf_model, -EPERM, ERR_INVALID_PTR);

    g_sf_model = _sensor_fusion_create();
    FORMULA_GUARD(g_sf_model == NULL, -EPERM, ERR_INVALID_PTR);

    return 0;
}

int sdpc_sensor_fusion_exit(void)
{
    FORMULA_GUARD(g_sf_model == NULL, -EPERM, ERR_INVALID_PTR);

    _sensor_fusion_destroy(g_sf_model);
    g_sf_model = NULL;

    return 0;
}

ExtY_sf_simulink_T *sensor_fusion_function(ExtU_sf_simulink_T *input)
{
    g_sf_model->inputs = input;
    sf_simulink_step(g_sf_model);

    return g_sf_model->outputs;
}

static RT_MODEL_sf_simulink_T *_sensor_fusion_create(void)
{
    RT_MODEL_sf_simulink_T *new = malloc(sizeof(RT_MODEL_sf_simulink_T));
    FORMULA_GUARD(new == NULL, NULL, ERR_MEMORY_SHORTAGE);

    new = sf_simulink();
    FORMULA_GUARD_WITH_EXIT_FUNC(new == NULL, NULL, _sensor_fusion_destroy(new),
                                 "");

    /* Initialize model */
    sf_simulink_initialize(new);

    return new;
}

static int _sensor_fusion_destroy(RT_MODEL_sf_simulink_T *del)
{
    FORMULA_GUARD(del == NULL, -EPERM, ERR_INVALID_PTR);

    sf_simulink_terminate(del);
    PTR_FREE(del);

    return 0;
}
