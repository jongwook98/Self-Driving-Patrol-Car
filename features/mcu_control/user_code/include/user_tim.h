
#ifndef __U_TIM_H__
#define __U_TIM_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "user_main.h"

    void timer7_ISR(TIM_HandleTypeDef *htim);
    void timer9_motor_ISR(void);
    void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#ifdef __cplusplus
}
#endif

#endif /* __U_TIM_H__ */
