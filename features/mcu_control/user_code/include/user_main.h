
#ifndef __U_MAIN_H__
#define __U_MAIN_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <math.h>

#include "main.h"

#include "control_flow.h"

#include "user_struct.h"
#include "user_tim.h"
#include "user_usart.h"

    extern RT_MODEL_control_flow_T control_flow;
    extern B_control_flow_T control_flow_B;
    extern ExtU_control_flow_T control_flow_IN;
    extern ExtY_control_flow_T control_flow_OUT;
    extern DW_control_flow_T control_flow_DW;

    void main_init(void);
    void main_while(void);

#ifdef __cplusplus
}
#endif

#endif /* __U_MAIN_H__ */
