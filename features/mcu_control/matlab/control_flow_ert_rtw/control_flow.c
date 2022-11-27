/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: control_flow.c
 *
 * Code generated for Simulink model 'control_flow'.
 *
 * Model version                  : 1.260
 * Simulink Coder version         : 9.3 (R2020a) 18-Nov-2019
 * C/C++ source code generated on : Fri Sep 30 21:18:49 2022
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
#define control_flow_ang_max           (165.0F)
#define control_flow_ang_min           (75.0F)
#define control_flow_servo_prd_max     (4200.0F)
#define control_flow_servo_prd_min     (840.0F)

/* Named constants for Chart: '<S1>/velo_adjust' */
#define control_fl_IN_NO_ACTIVE_CHILD_k ((uint8_T)0U)
#define control_flow_IN_velocity_com   ((uint8_T)1U)
#define control_flow_IN_velocity_dec   ((uint8_T)2U)
#define control_flow_irq_prd           (0.01F)
#define control_flow_pulse_per_velo    (0.226205915F)

/* Named constants for Chart: '<S1>/Chart' */
#define control_fl_IN_NO_ACTIVE_CHILD_h ((uint8_T)0U)
#define control_flow_IN_blink_com      ((uint8_T)1U)
#define control_flow_IN_blink_init     ((uint8_T)2U)
#define control_flow_PRD               (25.0)

/* Named constants for Chart: '<S3>/motor pid' */
#define control_flow_IN_velo_com       ((uint8_T)1U)
#define control_flow_IN_velo_dec       ((uint8_T)2U)
#define control_flow_MOTOR_PRD_LIMIT   (16799.0F)
#define control_flow_kp                (5.0F)

const RETURN control_flow_rtZRETURN = {
  0U,                                  /* motor_dir_u8 */
  0U,                                  /* motor_val_u32 */
  0U,                                  /* servo_val_u32 */
  0U,                                  /* blinker_left_u8 */
  0U,                                  /* blinker_right_u8 */
  0U                                   /* brake_light_u8 */
} ;                                    /* RETURN ground */

int32_T div_s32_sat(int32_T numerator, int32_T denominator);
void mul_wide_s32(int32_T in0, int32_T in1, uint32_T *ptrOutBitsHi, uint32_T
                  *ptrOutBitsLo);
int32_T mul_s32_sat(int32_T a, int32_T b);
void control_flow_servo_dir_Init(B_servo_dir_control_flow_T *localB,
  DW_servo_dir_control_flow_T *localDW);
void control_flow_servo_dir(real32_T rtu_angle, B_servo_dir_control_flow_T
  *localB, DW_servo_dir_control_flow_T *localDW);
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

int32_T div_s32_sat(int32_T numerator, int32_T denominator)
{
  int32_T quotient;
  uint32_T tempAbsQuotient;
  boolean_T quotientNeedsNegation;
  if (denominator == 0) {
    quotient = numerator >= 0 ? MAX_int32_T : MIN_int32_T;

    /* Divide by zero handler */
  } else {
    quotientNeedsNegation = ((numerator < 0) != (denominator < 0));
    tempAbsQuotient = (numerator < 0 ? ~(uint32_T)numerator + 1U : (uint32_T)
                       numerator) / (denominator < 0 ? ~(uint32_T)denominator +
      1U : (uint32_T)denominator);
    if ((!quotientNeedsNegation) && (tempAbsQuotient >= 2147483647U)) {
      quotient = MAX_int32_T;
    } else if (quotientNeedsNegation && (tempAbsQuotient > 2147483647U)) {
      quotient = MIN_int32_T;
    } else {
      quotient = quotientNeedsNegation ? -(int32_T)tempAbsQuotient : (int32_T)
        tempAbsQuotient;
    }
  }

  return quotient;
}

void mul_wide_s32(int32_T in0, int32_T in1, uint32_T *ptrOutBitsHi, uint32_T
                  *ptrOutBitsLo)
{
  uint32_T absIn0;
  uint32_T absIn1;
  uint32_T in0Lo;
  uint32_T in0Hi;
  uint32_T in1Hi;
  uint32_T productHiLo;
  uint32_T productLoHi;
  absIn0 = in0 < 0 ? ~(uint32_T)in0 + 1U : (uint32_T)in0;
  absIn1 = in1 < 0 ? ~(uint32_T)in1 + 1U : (uint32_T)in1;
  in0Hi = absIn0 >> 16U;
  in0Lo = absIn0 & 65535U;
  in1Hi = absIn1 >> 16U;
  absIn0 = absIn1 & 65535U;
  productHiLo = in0Hi * absIn0;
  productLoHi = in0Lo * in1Hi;
  absIn0 *= in0Lo;
  absIn1 = 0U;
  in0Lo = (productLoHi << /*MW:OvBitwiseOk*/ 16U) + /*MW:OvCarryOk*/ absIn0;
  if (in0Lo < absIn0) {
    absIn1 = 1U;
  }

  absIn0 = in0Lo;
  in0Lo += /*MW:OvCarryOk*/ productHiLo << /*MW:OvBitwiseOk*/ 16U;
  if (in0Lo < absIn0) {
    absIn1++;
  }

  absIn0 = (((productLoHi >> 16U) + (productHiLo >> 16U)) + in0Hi * in1Hi) +
    absIn1;
  if ((in0 != 0) && ((in1 != 0) && ((in0 > 0) != (in1 > 0)))) {
    absIn0 = ~absIn0;
    in0Lo = ~in0Lo;
    in0Lo++;
    if (in0Lo == 0U) {
      absIn0++;
    }
  }

  *ptrOutBitsHi = absIn0;
  *ptrOutBitsLo = in0Lo;
}

int32_T mul_s32_sat(int32_T a, int32_T b)
{
  int32_T result;
  uint32_T u32_chi;
  uint32_T u32_clo;
  mul_wide_s32(a, b, &u32_chi, &u32_clo);
  if (((int32_T)u32_chi > 0) || ((u32_chi == 0U) && (u32_clo >= 2147483648U))) {
    result = MAX_int32_T;
  } else if (((int32_T)u32_chi < -1) || (((int32_T)u32_chi == -1) && (u32_clo <
               2147483648U))) {
    result = MIN_int32_T;
  } else {
    result = (int32_T)u32_clo;
  }

  return result;
}

/* System initialize for atomic system: '<S1>/servo_dir' */
void control_flow_servo_dir_Init(B_servo_dir_control_flow_T *localB,
  DW_servo_dir_control_flow_T *localDW)
{
  localDW->is_active_c3_control_flow = 0U;
  localDW->is_c3_control_flow = control_flow_IN_NO_ACTIVE_CHILD;
  localB->servo_temp = 0U;
}

/* Output and update for atomic system: '<S1>/servo_dir' */
void control_flow_servo_dir(real32_T rtu_angle, B_servo_dir_control_flow_T
  *localB, DW_servo_dir_control_flow_T *localDW)
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
    ang_temp = (90.0F - rtu_angle) + 120.0F;
    if ((90.0F - rtu_angle) + 120.0F > control_flow_ang_max) {
      ang_temp = control_flow_ang_max;
    } else {
      if ((90.0F - rtu_angle) + 120.0F < control_flow_ang_min) {
        ang_temp = control_flow_ang_min;
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
  localDW->skip_prd_cnt = 0;
  localB->encoder_velo = 0;
}

/* Output and update for atomic system: '<S1>/velo_adjust' */
void control_flow_velo_adjust(uint16_T rtu_encoder, B_velo_adjust_control_flow_T
  *localB, DW_velo_adjust_control_flow_T *localDW)
{
  int32_T encoder_temp;
  real32_T tmp;

  /* Chart: '<S1>/velo_adjust' */
  if (localDW->is_active_c1_control_flow == 0U) {
    localDW->is_active_c1_control_flow = 1U;
    localDW->is_c1_control_flow = control_flow_IN_velocity_dec;
    localB->encoder_velo = 0;
    localDW->skip_prd_cnt = 0;
  } else if (localDW->is_c1_control_flow == control_flow_IN_velocity_com) {
    if (rtu_encoder > 32768) {
      encoder_temp = rtu_encoder - 65536;
    } else {
      encoder_temp = rtu_encoder;
    }

    if (localDW->skip_prd_cnt > 2147483646) {
      localDW->skip_prd_cnt = MAX_int32_T;
    } else {
      localDW->skip_prd_cnt++;
    }

    if (encoder_temp == 0) {
      if (localDW->skip_prd_cnt > 2147483646) {
        encoder_temp = MAX_int32_T;
      } else {
        encoder_temp = localDW->skip_prd_cnt + 1;
      }

      localB->encoder_velo = div_s32_sat(mul_s32_sat(localB->encoder_velo,
        localDW->skip_prd_cnt), encoder_temp);
    } else {
      tmp = (real32_T)encoder_temp * control_flow_pulse_per_velo / ((real32_T)
        localDW->skip_prd_cnt * control_flow_irq_prd);
      if (tmp < 2.14748365E+9F) {
        if (tmp >= -2.14748365E+9F) {
          localB->encoder_velo = (int32_T)tmp;
        } else {
          localB->encoder_velo = MIN_int32_T;
        }
      } else {
        localB->encoder_velo = MAX_int32_T;
      }

      localDW->skip_prd_cnt = 0;
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
  boolean_T guard1 = false;

  /* Chart: '<S1>/velo_adjust' incorporates:
   *  Inport: '<Root>/Input2'
   */
  control_flow_velo_adjust(control_flow_U->Input2.encoder_u16,
    &control_flow_B->sf_velo_adjust, &control_flow_DW->sf_velo_adjust);

  /* Chart: '<S3>/motor pid' incorporates:
   *  Inport: '<Root>/Input1'
   */
  if (control_flow_DW->is_active_c2_control_flow == 0U) {
    control_flow_DW->is_active_c2_control_flow = 1U;
    control_flow_DW->is_c2_control_flow = control_flow_IN_velo_dec;
    control_flow_DW->pid_val = 0.0F;
    control_flow_B->direction = 0U;
    control_flow_B->motor_val = 0U;
    control_flow_B->brake = 0U;
  } else if (control_flow_DW->is_c2_control_flow == control_flow_IN_velo_com) {
    err_velo = control_flow_U->Input1.input_velo_r32 - (real32_T)
      control_flow_B->sf_velo_adjust.encoder_velo;
    control_flow_DW->pid_val += control_flow_kp * err_velo;
    if (control_flow_U->Input1.input_velo_r32 == 0.0F) {
      control_flow_B->brake = 1U;
    } else if (control_flow_DW->pid_val > 0.0F) {
      if (err_velo < -25.0F) {
        control_flow_B->brake = 1U;
      } else {
        control_flow_B->brake = 0U;
      }
    } else if (err_velo > 25.0F) {
      control_flow_B->brake = 1U;
    } else {
      control_flow_B->brake = 0U;
    }

    if (control_flow_DW->pid_val < 0.0F) {
      if (-control_flow_DW->pid_val < 4.2949673E+9F) {
        if (-control_flow_DW->pid_val >= 0.0F) {
          control_flow_B->motor_val = (uint32_T)-control_flow_DW->pid_val;
        } else {
          control_flow_B->motor_val = 0U;
        }
      } else {
        control_flow_B->motor_val = MAX_uint32_T;
      }

      control_flow_B->direction = 0U;
    } else {
      if (control_flow_DW->pid_val < 4.2949673E+9F) {
        if (control_flow_DW->pid_val >= 0.0F) {
          control_flow_B->motor_val = (uint32_T)control_flow_DW->pid_val;
        } else {
          control_flow_B->motor_val = 0U;
        }
      } else {
        control_flow_B->motor_val = MAX_uint32_T;
      }

      control_flow_B->direction = 1U;
    }

    if (control_flow_B->motor_val > control_flow_MOTOR_PRD_LIMIT) {
      control_flow_B->motor_val = 16799U;
    }

    control_flow_DW->is_c2_control_flow = control_flow_IN_velo_com;
  } else {
    /* case IN_velo_dec: */
    control_flow_DW->is_c2_control_flow = control_flow_IN_velo_com;
  }

  /* End of Chart: '<S3>/motor pid' */

  /* Chart: '<S1>/servo_dir' incorporates:
   *  Inport: '<Root>/Input1'
   */
  control_flow_servo_dir(control_flow_U->Input1.input_angle_r32,
    &control_flow_B->sf_servo_dir, &control_flow_DW->sf_servo_dir);

  /* Chart: '<S1>/Chart' incorporates:
   *  Inport: '<Root>/Input'
   */
  if (control_flow_DW->is_active_c4_control_flow == 0U) {
    control_flow_DW->is_active_c4_control_flow = 1U;
    control_flow_DW->is_c4_control_flow = control_flow_IN_blink_init;
    control_flow_DW->blink_cnt = control_flow_PRD;
    control_flow_DW->blink_onoff = 0.0;
    control_flow_B->blink_left = 0U;
    control_flow_B->blink_right = 0U;
  } else if (control_flow_DW->is_c4_control_flow == control_flow_IN_blink_com) {
    guard1 = false;
    if (control_flow_U->flag.turn == 1) {
      if (control_flow_DW->blink_cnt >= control_flow_PRD) {
        control_flow_DW->blink_cnt = 0.0;
        if (control_flow_DW->blink_onoff == 0.0) {
          control_flow_DW->blink_onoff = 1.0;
          if (control_flow_U->flag.steer == 1) {
            control_flow_B->blink_left = 1U;
            control_flow_B->blink_right = 0U;
          } else {
            /*  [dir_flag == 0]  */
            control_flow_B->blink_left = 0U;
            control_flow_B->blink_right = 1U;
          }
        } else {
          control_flow_DW->blink_onoff = 0.0;
          guard1 = true;
        }
      } else {
        control_flow_DW->blink_cnt++;
      }
    } else {
      control_flow_DW->blink_cnt = control_flow_PRD;
      control_flow_DW->blink_onoff = 0.0;
      guard1 = true;
    }

    if (guard1) {
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
   *  Outport: '<Root>/Output1'
   */
  control_flow_Y->Output1.motor_dir_u8 = control_flow_B->direction;
  control_flow_Y->Output1.motor_val_u32 = control_flow_B->motor_val;
  control_flow_Y->Output1.servo_val_u32 =
    control_flow_B->sf_servo_dir.servo_temp;
  control_flow_Y->Output1.blinker_left_u8 = control_flow_B->blink_left;
  control_flow_Y->Output1.blinker_right_u8 = control_flow_B->blink_right;
  control_flow_Y->Output1.brake_light_u8 = control_flow_B->brake;
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

  /* SystemInitialize for Chart: '<S3>/motor pid' */
  control_flow_DW->is_active_c2_control_flow = 0U;
  control_flow_DW->is_c2_control_flow = control_fl_IN_NO_ACTIVE_CHILD_h;
  control_flow_DW->pid_val = 0.0F;
  control_flow_B->direction = 0U;
  control_flow_B->motor_val = 0U;
  control_flow_B->brake = 0U;

  /* SystemInitialize for Chart: '<S1>/servo_dir' */
  control_flow_servo_dir_Init(&control_flow_B->sf_servo_dir,
    &control_flow_DW->sf_servo_dir);

  /* SystemInitialize for Chart: '<S1>/Chart' */
  control_flow_DW->is_active_c4_control_flow = 0U;
  control_flow_DW->is_c4_control_flow = control_fl_IN_NO_ACTIVE_CHILD_h;
  control_flow_DW->blink_cnt = 0.0;
  control_flow_DW->blink_onoff = 0.0;
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
