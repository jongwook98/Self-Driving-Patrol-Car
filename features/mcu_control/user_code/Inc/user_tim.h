
#ifndef __U_TIM_H__
#define __U_TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "user_main.h"

extern RT_MODEL_control_flow_T control_flow;
extern B_control_flow_T control_flow_B;
extern ExtU_control_flow_T control_flow_IN;
extern ExtY_control_flow_T control_flow_OUT;
extern DW_control_flow_T control_flow_DW;

void timer7_ISR(TIM_HandleTypeDef *htim);
void timer9_motor_ISR(void);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#ifdef __cplusplus
}
#endif

#endif /* __U_TIM_H__ */


