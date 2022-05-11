/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: ert_main.c
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
#include "rtwtypes.h"

static RT_MODEL_control_flow_T control_flow_M_;
static RT_MODEL_control_flow_T *const control_flow_MPtr = &control_flow_M_;/* Real-time model */
static B_control_flow_T control_flow_B;/* Observable signals */
static DW_control_flow_T control_flow_DW;/* Observable states */
static ExtU_control_flow_T control_flow_U;/* External inputs */
static ExtY_control_flow_T control_flow_Y;/* External outputs */
volatile int IsrOverrun = 0;
static boolean_T OverrunFlag = 0;
void rt_OneStep(void)
{
  /* Check for overrun. Protect OverrunFlag against preemption */
  if (OverrunFlag++) {
    IsrOverrun = 1;
    OverrunFlag--;
    return;
  }

  __enable_irq();
  control_flow_step(control_flow_M);

  /* Get model outputs here */
  __disable_irq();
  OverrunFlag--;
}

volatile boolean_T stopRequested;
volatile boolean_T runModel;
int main(int argc, char **argv)
{
  float modelBaseRate = 0.1;
  float systemClock = 168;

  /* Initialize variables */
  stopRequested = false;
  runModel = false;

#ifndef USE_RTX
#if defined(MW_MULTI_TASKING_MODE) && (MW_MULTI_TASKING_MODE == 1)

  MW_ASM (" SVC #1");

#endif

  __disable_irq();

#endif

  ;
  stm32f4xx_init_board();
  SystemCoreClockUpdate();
  bootloaderInit();
  ((void) 0);
  control_flow_initialize(control_flow_M);
  ARMCM_SysTick_Config(modelBaseRate);
  runModel = true;
  __enable_irq();
  __enable_irq();
  while (runModel) {
  }

  /* Disable rt_OneStep() here */

  /* Terminate model */
  control_flow_terminate(control_flow_M);

#ifndef USE_RTX

  (void)systemClock;

#endif

  ;
  __disable_irq();
  return 0;
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
