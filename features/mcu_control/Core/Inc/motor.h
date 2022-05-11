/*
 * motor.h
 *
 *  Created on: Mar 11, 2022
 *      Author: kimjs
 */

#ifndef __MOTOR_H__
#define __MOTOR_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "main.h"

extern RT_MODEL_control_flow_T control_flow;
extern B_control_flow_T control_flow_B;
extern ExtU_control_flow_T control_flow_IN;
extern ExtY_control_flow_T control_flow_OUT;
extern DW_control_flow_T control_flow_DW;

extern bit_field_flag_t g_Flag;
extern motor_vari g_motor;


void init_motor_variable( motor_vari *pm );
void timer9_motor_ISR(void);


#ifdef __cplusplus
}
#endif

#endif /* __MOTOR_H__ */
