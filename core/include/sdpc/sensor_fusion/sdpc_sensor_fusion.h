/* vim: set tabstop=4:softtabstop=4:shiftwidth=4:ffs=unix */
#ifndef SDPC_SENSOR_FUSION_H
#define SDPC_SENSOR_FUSION_H

#include <sf_simulink.h>

RT_MODEL_sf_simulink_T *sensor_fusion_create(void);
int sensor_fusion_destroy(RT_MODEL_sf_simulink_T **const del);

#endif /* SDPC_SENSOR_FUSION_H */
