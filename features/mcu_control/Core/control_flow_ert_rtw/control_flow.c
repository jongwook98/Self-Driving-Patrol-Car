/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: control_flow.c
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

#include "control_flow.h"

/* Named constants for Chart: '<S1>/servo_dir' */
#define control_flow_IN_NO_ACTIVE_CHILD ((uint8_T)0U)
#define control_flow_IN_angle_dec      ((uint8_T)1U)
#define control_flow_IN_direction_dec  ((uint8_T)2U)
#define control_flow_ang_max           (150.0F)
#define control_flow_ang_min           (90.0F)
#define control_flow_servo_prd_max     (4200.0F)
#define control_flow_servo_prd_min     (840.0F)

/* Named constants for Chart: '<S1>/velo_adjust' */
#define control_fl_IN_NO_ACTIVE_CHILD_k ((uint8_T)0U)
#define control_flow_IN_velocity_com   ((uint8_T)1U)
#define control_flow_IN_velocity_dec   ((uint8_T)2U)
#define control_flow_MAX_uint16_T_2    (32767)
#define control_flow_irq_prd           (0.0005F)
#define control_flow_pulse_per_velo    (0.101654485F)

/* Named constants for Chart: '<S1>/Chart' */
#define control_fl_IN_NO_ACTIVE_CHILD_h ((uint8_T)0U)
#define control_flow_IN_blink_com      ((uint8_T)1U)
#define control_flow_IN_blink_init     ((uint8_T)2U)

/* Named constants for Chart: '<S3>/Chart' */
#define control_flow_IN_velo_com       ((uint8_T)1U)
#define control_flow_IN_velo_dec       ((uint8_T)2U)
#define control_flow_kd                (7.5)
#define control_flow_kp                (10.0)

const RETURN control_flow_rtZRETURN = {
  0U,                                  /* motor_dir_u8 */
  0U,                                  /* motor_val_u32 */
  0U,                                  /* servo_val_u32 */
  0U,                                  /* blinker_left_u8 */
  0U,                                  /* blinker_right_u8 */
  0U                                   /* start_stop_u8 */
} ;                                    /* RETURN ground */

void control_flow_servo_dir_Init(B_servo_dir_control_flow_T *localB,
  DW_servo_dir_control_flow_T *localDW);
void control_flow_servo_dir(real32_T rtu_angle, uint8_T rtu_turn_able_flag,
  B_servo_dir_control_flow_T *localB, DW_servo_dir_control_flow_T *localDW);
void control_flow_velo_adjust_Init(B_velo_adjust_control_flow_T *localB,
  DW_velo_adjust_control_flow_T *localDW);
void control_flow_velo_adjust(uint16_T rtu_encoder, B_velo_adjust_control_flow_T
  *localB, DW_velo_adjust_control_flow_T *localDW);

/*===========*
 * Constants *
 *===========*/
#define RT_PI                          3.14159265358979323846
#define RT_PIF                         3.1415927F
#define RT_LN_10                       2.30258509299404568402
#define RT_LN_10F                      2.3025851F
#define RT_LOG10E                      0.43429448190325182765
#define RT_LOG10EF                     0.43429449F
#define RT_E                           2.7182818284590452354
#define RT_EF                          2.7182817F

/*
 * UNUSED_PARAMETER(x)
 *   Used to specify that a function parameter (argument) is required but not
 *   accessed by the function body.
 */
#ifndef UNUSED_PARAMETER
# if defined(__LCC__)
#   define UNUSED_PARAMETER(x)                                   /* do nothing */
# else

/*
 * This is the semi-ANSI standard way of indicating that an
 * unused function parameter is required.
 */
#   define UNUSED_PARAMETER(x)         (void) (x)
# endif
#endif

/* System initialize for atomic system: '<S1>/servo_dir' */
void control_flow_servo_dir_Init(B_servo_dir_control_flow_T *localB,
  DW_servo_dir_control_flow_T *localDW)
{
  localDW->is_active_c3_control_flow = 0U;
  localDW->is_c3_control_flow = control_flow_IN_NO_ACTIVE_CHILD;
  localB->servo_temp = 0U;
}

/* Output and update for atomic system: '<S1>/servo_dir' */
void control_flow_servo_dir(real32_T rtu_angle, uint8_T rtu_turn_able_flag,
  B_servo_dir_control_flow_T *localB, DW_servo_dir_control_flow_T *localDW)
{
  real32_T ang_temp;

  /* Chart: '<S1>/servo_dir' */
  if (localDW->is_active_c3_control_flow == 0U) {
    localDW->is_active_c3_control_flow = 1U;
    localDW->is_c3_control_flow = control_flow_IN_angle_dec;
    localB->servo_temp = 0U;
  } else if (localDW->is_c3_control_flow == control_flow_IN_angle_dec) {
    localDW->is_c3_control_flow = control_flow_IN_direction_dec;
  } else {
    /* case IN_direction_dec: */
    if (rtu_turn_able_flag == 1) {
      ang_temp = control_flow_ang_min;
    } else if (rtu_turn_able_flag == 2) {
      ang_temp = control_flow_ang_max;
    } else {
      ang_temp = rtu_angle + 120.0F;
      if (rtu_angle + 120.0F > control_flow_ang_max) {
        ang_temp = control_flow_ang_max;
      } else {
        if (rtu_angle + 120.0F < control_flow_ang_min) {
          ang_temp = control_flow_ang_min;
        }
      }
    }

    ang_temp = 18.666666F * ang_temp + control_flow_servo_prd_min;
    if (ang_temp < 4.2949673E+9F) {
      if (ang_temp >= 0.0F) {
        localB->servo_temp = (uint32_T)ang_temp;
      } else {
        localB->servo_temp = 0U;
      }
    } else {
      localB->servo_temp = MAX_uint32_T;
    }

    if (localB->servo_temp > control_flow_servo_prd_max) {
      localB->servo_temp = 4200U;
    } else {
      if (localB->servo_temp < control_flow_servo_prd_min) {
        localB->servo_temp = 840U;
      }
    }

    localDW->is_c3_control_flow = control_flow_IN_direction_dec;
  }

  /* End of Chart: '<S1>/servo_dir' */
}

/* System initialize for atomic system: '<S1>/velo_adjust' */
void control_flow_velo_adjust_Init(B_velo_adjust_control_flow_T *localB,
  DW_velo_adjust_control_flow_T *localDW)
{
  localDW->is_active_c1_control_flow = 0U;
  localDW->is_c1_control_flow = control_fl_IN_NO_ACTIVE_CHILD_k;
  localDW->skip_prd_cnt = 0U;
  localDW->avar_velo[0] = 0.0F;
  localDW->avar_velo[1] = 0.0F;
  localDW->avar_velo[2] = 0.0F;
  localDW->avar_velo[3] = 0.0F;
  localB->encoder_velo = 0.0F;
}

/* Output and update for atomic system: '<S1>/velo_adjust' */
void control_flow_velo_adjust(uint16_T rtu_encoder, B_velo_adjust_control_flow_T
  *localB, DW_velo_adjust_control_flow_T *localDW)
{
  int32_T encoder_temp;
  uint32_T qY;

  /* Chart: '<S1>/velo_adjust' */
  if (localDW->is_active_c1_control_flow == 0U) {
    localDW->is_active_c1_control_flow = 1U;
    localDW->is_c1_control_flow = control_flow_IN_velocity_dec;
    localB->encoder_velo = 0.0F;
    localDW->skip_prd_cnt = 0U;
    localDW->avar_velo[0] = 0.0F;
    localDW->avar_velo[1] = 0.0F;
    localDW->avar_velo[2] = 0.0F;
    localDW->avar_velo[3] = 0.0F;
  } else if (localDW->is_c1_control_flow == control_flow_IN_velocity_com) {
    encoder_temp = rtu_encoder;
    qY = localDW->skip_prd_cnt + /*MW:OvSatOk*/ 1U;
    if (qY < localDW->skip_prd_cnt) {
      qY = MAX_uint32_T;
    }

    localDW->skip_prd_cnt = qY;
    if (rtu_encoder == 0) {
      localB->encoder_velo = 0.0F / ((real32_T)localDW->skip_prd_cnt *
        control_flow_irq_prd);
    } else {
      if (rtu_encoder > control_flow_MAX_uint16_T_2) {
        encoder_temp = rtu_encoder - 65536;
      }

      localDW->avar_velo[3] = localDW->avar_velo[2];
      localDW->avar_velo[2] = localDW->avar_velo[1];
      localDW->avar_velo[1] = localDW->avar_velo[0];
      localDW->avar_velo[0] = (real32_T)encoder_temp *
        control_flow_pulse_per_velo / ((real32_T)localDW->skip_prd_cnt *
        control_flow_irq_prd);
      localB->encoder_velo = (real32_T)((((localDW->avar_velo[3] +
        localDW->avar_velo[2]) + localDW->avar_velo[1]) + localDW->avar_velo[0])
        * 0.25);
      localDW->skip_prd_cnt = 0U;
    }

    localDW->is_c1_control_flow = control_flow_IN_velocity_com;
  } else {
    /* case IN_velocity_dec: */
    localDW->is_c1_control_flow = control_flow_IN_velocity_com;
  }

  /* End of Chart: '<S1>/velo_adjust' */
}

/* Model step function */
void control_flow_step(RT_MODEL_control_flow_T *const control_flow_M)
{
  B_control_flow_T *control_flow_B = ((B_control_flow_T *)
    control_flow_M->blockIO);
  DW_control_flow_T *control_flow_DW = ((DW_control_flow_T *)
    control_flow_M->dwork);
  ExtU_control_flow_T *control_flow_U = (ExtU_control_flow_T *)
    control_flow_M->inputs;
  ExtY_control_flow_T *control_flow_Y = (ExtY_control_flow_T *)
    control_flow_M->outputs;
  real32_T err_velo;
  real32_T goal_local;
  uint16_T tmp;

  /* Chart: '<S1>/velo_adjust' incorporates:
   *  Inport: '<Root>/Input2'
   */
  control_flow_velo_adjust(control_flow_U->Input2.encoder_u16,
    &control_flow_B->sf_velo_adjust, &control_flow_DW->sf_velo_adjust);

  /* Chart: '<S3>/Chart' incorporates:
   *  Inport: '<Root>/Input'
   *  Inport: '<Root>/Input1'
   *  Inport: '<Root>/Input2'
   */
  if (control_flow_DW->is_active_c2_control_flow == 0U) {
    control_flow_DW->is_active_c2_control_flow = 1U;
    control_flow_DW->is_c2_control_flow = control_flow_IN_velo_dec;
    control_flow_DW->pid_i = 0.0;
    control_flow_DW->pid_d[0] = 0.0F;
    control_flow_DW->pid_d[1] = 0.0F;
    control_flow_DW->pid_d[2] = 0.0F;
    control_flow_B->PID = 0.0F;
    control_flow_B->direction = 0U;
  } else if (control_flow_DW->is_c2_control_flow == control_flow_IN_velo_com) {
    if (control_flow_U->Input1.input_velo_r64 > 300.0F) {
      goal_local = 300.0F;
    } else if (control_flow_U->Input1.input_velo_r64 < -300.0F) {
      goal_local = -300.0F;
    } else {
      goal_local = control_flow_U->Input1.input_velo_r64;
    }

    if (control_flow_U->flag.move != 1) {
      goal_local = 0.0F;
    }

    err_velo = goal_local - control_flow_B->sf_velo_adjust.encoder_velo;
    control_flow_DW->pid_i += err_velo;
    control_flow_DW->pid_d[2] = control_flow_DW->pid_d[1];
    control_flow_DW->pid_d[1] = err_velo;
    control_flow_DW->pid_d[0] = control_flow_DW->pid_d[2] -
      control_flow_DW->pid_d[1];
    control_flow_B->PID = (real32_T)(((control_flow_kp * err_velo + goal_local)
      + control_flow_DW->pid_i) + control_flow_kd * control_flow_DW->pid_d[0]);
    if (control_flow_B->PID > 0.0F) {
      control_flow_B->direction = 1U;
    } else if (control_flow_B->PID < 0.0F) {
      control_flow_B->direction = 0U;
    } else {
      tmp = control_flow_U->Input2.motor_dir_u16;
      if (control_flow_U->Input2.motor_dir_u16 > 255) {
        tmp = 255U;
      }

      control_flow_B->direction = (uint8_T)tmp;
    }

    control_flow_B->PID = fabsf(control_flow_B->PID);
    if (control_flow_B->PID > 1679.0F) {
      control_flow_B->PID = 1679.0F;
    }

    control_flow_DW->is_c2_control_flow = control_flow_IN_velo_com;
  } else {
    /* case IN_velo_dec: */
    control_flow_DW->is_c2_control_flow = control_flow_IN_velo_com;
  }

  /* End of Chart: '<S3>/Chart' */

  /* DataTypeConversion: '<S3>/Data Type Conversion' */
  goal_local = fmodf(floorf(control_flow_B->PID), 4.2949673E+9F);
  control_flow_Y->Output1.motor_val_u32 = goal_local < 0.0F ? (uint32_T)
    -(int32_T)(uint32_T)-goal_local : (uint32_T)goal_local;

  /* Chart: '<S1>/servo_dir' incorporates:
   *  Inport: '<Root>/Input'
   *  Inport: '<Root>/Input1'
   */
  control_flow_servo_dir(control_flow_U->Input1.input_angle_r64,
    control_flow_U->flag.turn_move, &control_flow_B->sf_servo_dir,
    &control_flow_DW->sf_servo_dir);

  /* Chart: '<S1>/Chart' incorporates:
   *  Inport: '<Root>/Input'
   */
  if (control_flow_DW->is_active_c4_control_flow == 0U) {
    control_flow_DW->is_active_c4_control_flow = 1U;
    control_flow_DW->is_c4_control_flow = control_flow_IN_blink_init;
    control_flow_B->blink_left = 0U;
    control_flow_B->blink_right = 0U;
  } else if (control_flow_DW->is_c4_control_flow == control_flow_IN_blink_com) {
    if (control_flow_U->flag.blinker == 1) {
      control_flow_B->blink_left = 0U;
      control_flow_B->blink_right = 1U;
    } else if (control_flow_U->flag.blinker == 2) {
      control_flow_B->blink_left = 1U;
      control_flow_B->blink_right = 0U;
    } else {
      control_flow_B->blink_left = 0U;
      control_flow_B->blink_right = 0U;
    }

    control_flow_DW->is_c4_control_flow = control_flow_IN_blink_com;
  } else {
    /* case IN_blink_init: */
    control_flow_DW->is_c4_control_flow = control_flow_IN_blink_com;
  }

  /* End of Chart: '<S1>/Chart' */

  /* BusCreator: '<S1>/Bus Creator' incorporates:
   *  Inport: '<Root>/Input'
   *  Outport: '<Root>/Output1'
   */
  control_flow_Y->Output1.motor_dir_u8 = control_flow_B->direction;
  control_flow_Y->Output1.servo_val_u32 =
    control_flow_B->sf_servo_dir.servo_temp;
  control_flow_Y->Output1.blinker_left_u8 = control_flow_B->blink_left;
  control_flow_Y->Output1.blinker_right_u8 = control_flow_B->blink_right;
  control_flow_Y->Output1.start_stop_u8 = control_flow_U->flag.move;
}

/* Model initialize function */
void control_flow_initialize(RT_MODEL_control_flow_T *const control_flow_M)
{
  B_control_flow_T *control_flow_B = ((B_control_flow_T *)
    control_flow_M->blockIO);
  DW_control_flow_T *control_flow_DW = ((DW_control_flow_T *)
    control_flow_M->dwork);
  ExtU_control_flow_T *control_flow_U = (ExtU_control_flow_T *)
    control_flow_M->inputs;
  ExtY_control_flow_T *control_flow_Y = (ExtY_control_flow_T *)
    control_flow_M->outputs;

  /* Registration code */

  /* block I/O */
  (void) memset(((void *) control_flow_B), 0,
                sizeof(B_control_flow_T));

  /* states (dwork) */
  (void) memset((void *)control_flow_DW, 0,
                sizeof(DW_control_flow_T));

  /* external inputs */
  (void)memset(control_flow_U, 0, sizeof(ExtU_control_flow_T));

  /* external outputs */
  control_flow_Y->Output1 = control_flow_rtZRETURN;

  /* SystemInitialize for Chart: '<S1>/velo_adjust' */
  control_flow_velo_adjust_Init(&control_flow_B->sf_velo_adjust,
    &control_flow_DW->sf_velo_adjust);

  /* SystemInitialize for Chart: '<S3>/Chart' */
  control_flow_DW->is_active_c2_control_flow = 0U;
  control_flow_DW->is_c2_control_flow = control_fl_IN_NO_ACTIVE_CHILD_h;
  control_flow_DW->pid_d[0] = 0.0F;
  control_flow_DW->pid_d[1] = 0.0F;
  control_flow_DW->pid_d[2] = 0.0F;
  control_flow_DW->pid_i = 0.0;
  control_flow_B->direction = 0U;
  control_flow_B->PID = 0.0F;

  /* SystemInitialize for Chart: '<S1>/servo_dir' */
  control_flow_servo_dir_Init(&control_flow_B->sf_servo_dir,
    &control_flow_DW->sf_servo_dir);

  /* SystemInitialize for Chart: '<S1>/Chart' */
  control_flow_DW->is_active_c4_control_flow = 0U;
  control_flow_DW->is_c4_control_flow = control_fl_IN_NO_ACTIVE_CHILD_h;
  control_flow_B->blink_right = 0U;
  control_flow_B->blink_left = 0U;
}

/* Model terminate function */
void control_flow_terminate(RT_MODEL_control_flow_T *const control_flow_M)
{
  /* (no terminate code required) */
  UNUSED_PARAMETER(control_flow_M);
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
