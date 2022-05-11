/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: control_flow.h
 *
 * Code generated for Simulink model 'control_flow'.
 *
 * Model version                  : 1.159
 * Simulink Coder version         : 9.3 (R2020a) 18-Nov-2019
 * C/C++ source code generated on : Wed May  4 03:15:37 2022
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_control_flow_h_
#define RTW_HEADER_control_flow_h_
#include "rtwtypes.h"
#include <math.h>
#include <string.h>
#ifndef control_flow_COMMON_INCLUDES_
# define control_flow_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "rtw_continuous.h"
#include "rtw_solver.h"
#endif                                 /* control_flow_COMMON_INCLUDES_ */

/* Model Code Variants */
#include "MW_target_hardware_resources.h"

/* Macros for accessing real-time model data structure */
#ifndef rtmGetBlockIO
# define rtmGetBlockIO(rtm)            ((rtm)->blockIO)
#endif

#ifndef rtmSetBlockIO
# define rtmSetBlockIO(rtm, val)       ((rtm)->blockIO = (val))
#endif

#ifndef rtmGetRootDWork
# define rtmGetRootDWork(rtm)          ((rtm)->dwork)
#endif

#ifndef rtmSetRootDWork
# define rtmSetRootDWork(rtm, val)     ((rtm)->dwork = (val))
#endif

#ifndef rtmGetU
# define rtmGetU(rtm)                  ((rtm)->inputs)
#endif

#ifndef rtmSetU
# define rtmSetU(rtm, val)             ((rtm)->inputs = (val))
#endif

#ifndef rtmGetY
# define rtmGetY(rtm)                  ((rtm)->outputs)
#endif

#ifndef rtmSetY
# define rtmSetY(rtm, val)             ((rtm)->outputs = (val))
#endif

/* Forward declaration for rtModel */
typedef struct tag_RTM_control_flow_T RT_MODEL_control_flow_T;

#ifndef DEFINED_TYPEDEF_FOR_FROM_LATTE_
#define DEFINED_TYPEDEF_FOR_FROM_LATTE_

typedef struct {
  real32_T input_angle_r64;
  real32_T input_velo_r64;
} FROM_LATTE;

#endif

#ifndef DEFINED_TYPEDEF_FOR_MOTOR_CONTROL_
#define DEFINED_TYPEDEF_FOR_MOTOR_CONTROL_

typedef struct {
  uint16_T encoder_u16;
  uint16_T motor_dir_u16;
  uint32_T accel_u32;
} MOTOR_CONTROL;

#endif

#ifndef DEFINED_TYPEDEF_FOR_FROM_LATTE_FLAG_
#define DEFINED_TYPEDEF_FOR_FROM_LATTE_FLAG_

typedef struct {
  uint8_T blinker;
  uint8_T turn_move;
  uint8_T move;
} FROM_LATTE_FLAG;

#endif

#ifndef DEFINED_TYPEDEF_FOR_RETURN_
#define DEFINED_TYPEDEF_FOR_RETURN_

typedef struct {
  uint8_T motor_dir_u8;
  uint32_T motor_val_u32;
  uint32_T servo_val_u32;
  uint8_T blinker_left_u8;
  uint8_T blinker_right_u8;
  uint8_T start_stop_u8;
} RETURN;

#endif

/* Block signals for system '<S1>/servo_dir' */
typedef struct {
  uint32_T servo_temp;                 /* '<S1>/servo_dir' */
} B_servo_dir_control_flow_T;

/* Block states (default storage) for system '<S1>/servo_dir' */
typedef struct {
  uint8_T is_active_c3_control_flow;   /* '<S1>/servo_dir' */
  uint8_T is_c3_control_flow;          /* '<S1>/servo_dir' */
} DW_servo_dir_control_flow_T;

/* Block signals for system '<S1>/velo_adjust' */
typedef struct {
  real32_T encoder_velo;               /* '<S1>/velo_adjust' */
} B_velo_adjust_control_flow_T;

/* Block states (default storage) for system '<S1>/velo_adjust' */
typedef struct {
  real32_T avar_velo[4];               /* '<S1>/velo_adjust' */
  uint32_T skip_prd_cnt;               /* '<S1>/velo_adjust' */
  uint8_T is_active_c1_control_flow;   /* '<S1>/velo_adjust' */
  uint8_T is_c1_control_flow;          /* '<S1>/velo_adjust' */
} DW_velo_adjust_control_flow_T;

/* Block signals (default storage) */
typedef struct {
  real32_T PID;                        /* '<S3>/Chart' */
  uint8_T direction;                   /* '<S3>/Chart' */
  uint8_T blink_right;                 /* '<S1>/Chart' */
  uint8_T blink_left;                  /* '<S1>/Chart' */
  B_velo_adjust_control_flow_T sf_velo_adjust;/* '<S1>/velo_adjust' */
  B_servo_dir_control_flow_T sf_servo_dir;/* '<S1>/servo_dir' */
} B_control_flow_T;

/* Block states (default storage) for system '<Root>' */
typedef struct {
  real_T pid_i;                        /* '<S3>/Chart' */
  real32_T pid_d[3];                   /* '<S3>/Chart' */
  uint8_T is_active_c2_control_flow;   /* '<S3>/Chart' */
  uint8_T is_c2_control_flow;          /* '<S3>/Chart' */
  uint8_T is_active_c4_control_flow;   /* '<S1>/Chart' */
  uint8_T is_c4_control_flow;          /* '<S1>/Chart' */
  DW_velo_adjust_control_flow_T sf_velo_adjust;/* '<S1>/velo_adjust' */
  DW_servo_dir_control_flow_T sf_servo_dir;/* '<S1>/servo_dir' */
} DW_control_flow_T;

/* External inputs (root inport signals with default storage) */
typedef struct {
  FROM_LATTE Input1;                   /* '<Root>/Input1' */
  MOTOR_CONTROL Input2;                /* '<Root>/Input2' */
  FROM_LATTE_FLAG flag;                /* '<Root>/Input' */
} ExtU_control_flow_T;

/* External outputs (root outports fed by signals with default storage) */
typedef struct {
  RETURN Output1;                      /* '<Root>/Output1' */
} ExtY_control_flow_T;

/* Real-time Model Data Structure */
struct tag_RTM_control_flow_T {
  B_control_flow_T *blockIO;
  ExtU_control_flow_T *inputs;
  ExtY_control_flow_T *outputs;
  DW_control_flow_T *dwork;
};

/* External data declarations for dependent source files */
extern const RETURN control_flow_rtZRETURN;/* RETURN ground */

/* Model entry point functions */
void control_flow_initialize(RT_MODEL_control_flow_T *const control_flow_M);
void control_flow_step(RT_MODEL_control_flow_T *const control_flow_M);
void control_flow_terminate(RT_MODEL_control_flow_T *const control_flow_M);

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'control_flow'
 * '<S1>'   : 'control_flow/motor_system'
 * '<S2>'   : 'control_flow/motor_system/Chart'
 * '<S3>'   : 'control_flow/motor_system/motor_pid'
 * '<S4>'   : 'control_flow/motor_system/servo_dir'
 * '<S5>'   : 'control_flow/motor_system/velo_adjust'
 * '<S6>'   : 'control_flow/motor_system/motor_pid/Chart'
 */
#endif                                 /* RTW_HEADER_control_flow_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
