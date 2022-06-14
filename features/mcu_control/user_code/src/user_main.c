
#include "user_main.h"
#include "tim.h"

static inline void _hal_lib_init(void);
static inline void _matlab_var_init(void);

RT_MODEL_control_flow_T control_flow;
B_control_flow_T control_flow_B;
ExtU_control_flow_T control_flow_IN;
ExtY_control_flow_T control_flow_OUT;
DW_control_flow_T control_flow_DW;

void main_init(void)
{
    _hal_lib_init();
    _matlab_var_init();

    receive_uart_start_it();

    TxPrintf("\n-----\nRESET\n-----\n");
    TX_LED_OFF;
    RX_LED_OFF;
}

void main_while(void) {}

static inline void _hal_lib_init(void)
{
    HAL_TIM_Base_Start_IT(&htim9); // APB2 TIMER IT(168)
    HAL_TIM_Base_Start_IT(&htim7); // APB1 TIMER IT(84)

    HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1);

    HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_ALL);
    HAL_TIM_Base_Start_IT(&htim1);
}

static inline void _matlab_var_init(void)
{
    control_flow.blockIO = &control_flow_B;
    control_flow.inputs = &control_flow_IN;
    control_flow.outputs = &control_flow_OUT;
    control_flow.dwork = &control_flow_DW;

    control_flow_initialize(&control_flow);

    control_flow.inputs->Input1.input_angle_r32 = (real32_T)90;
    control_flow.inputs->Input1.input_velo_r32 = (real32_T)(-150);
}
