
#include "user_tim.h"
#include "tim.h"

void timer7_ISR(TIM_HandleTypeDef *htim) {}

void timer9_motor_ISR(void)
{
    MOTOR_CONTROL *input_val2 = &(control_flow.inputs->Input2);
    RETURN *output_val = &(control_flow.outputs->Output1);

    input_val2->encoder_u16 = (uint16_T)TIM8->CNT;
    TIM8->CNT = 0;
    input_val2->motor_dir_u16 =
        (uint16_T)HAL_GPIO_ReadPin(motor_dir_GPIO_Port, motor_dir_Pin);

    control_flow_step(&control_flow);

    HAL_GPIO_WritePin(blinker_left_GPIO_Port, blinker_left_Pin,
                      output_val->blinker_left_u8);
    HAL_GPIO_WritePin(blinker_right_GPIO_Port, blinker_right_Pin,
                      output_val->blinker_right_u8);
    HAL_GPIO_WritePin(break_light_GPIO_Port, break_light_Pin,
                      output_val->brake_light_u8);
    HAL_GPIO_WritePin(motor_dir_GPIO_Port, motor_dir_Pin,
                      output_val->motor_dir_u8);

    TIM10->CCR1 = (uint32_t)output_val->motor_val_u32;
    TIM11->CCR1 = (uint32_t)output_val->servo_val_u32;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM7)
        timer7_ISR(htim);

    if (htim->Instance == TIM9)
        timer9_motor_ISR();
}
