/* vim: set tabstop=4:softtabstop=4:shiftwidth=4:ffs=unix */
#ifndef SDPC_SENSOR_FUSION_H
#define SDPC_SENSOR_FUSION_H

#include <sf_simulink.h>

int sdpc_sensor_fusion_init(void);
int sdpc_sensor_fusion_exit(void);

ExtY_sf_simulink_T *sensor_fusion_function(ExtU_sf_simulink_T *input);

#endif /* SDPC_SENSOR_FUSION_H */
