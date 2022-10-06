#include <sdpc/common/sdpc_common.h>
#include <sdpc/sensor_fusion/sdpc_sensor_fusion.h>

RT_MODEL_sf_simulink_T *sensor_fusion_create(void)
{
    RT_MODEL_sf_simulink_T *new = malloc(sizeof(RT_MODEL_sf_simulink_T));
    FORMULA_GUARD(new == NULL, NULL, ERR_MEMORY_SHORTAGE);

    new = sf_simulink();
    FORMULA_GUARD_WITH_EXIT_FUNC(new == NULL, NULL, sensor_fusion_destroy(&new),
                                 "");

    /* Initialize model */
    sf_simulink_initialize(new);

    return new;
}

int sensor_fusion_destroy(RT_MODEL_sf_simulink_T **const model)
{
    FORMULA_GUARD(model == NULL || *model == NULL, -EPERM, ERR_INVALID_PTR);
    RT_MODEL_sf_simulink_T *del = *model;

    sf_simulink_terminate(del);
    PTR_FREE(del);

    return 0;
}
