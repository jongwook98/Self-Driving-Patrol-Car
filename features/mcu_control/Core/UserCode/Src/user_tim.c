
#include "tim.h"
#include "user_tim.h"

RT_MODEL_control_flow_T control_flow;
B_control_flow_T control_flow_B;
ExtU_control_flow_T control_flow_IN;
ExtY_control_flow_T control_flow_OUT;
DW_control_flow_T control_flow_DW;

void timer7_ISR(TIM_HandleTypeDef *htim)
{
	// htim->Instance->ARR 		// counter period(auto-reload register) set
	// htim->Instance->psc 		// prescaler set

	//HAL_GPIO_TogglePin(PD7_LED_GPIO_Port, PD7_LED_Pin);
	//HAL_GPIO_TogglePin(PB7_MOTOR_DIR_GPIO_Port, PB7_MOTOR_DIR_Pin);
}

void timer9_motor_ISR(void)
{
	// htim->Instance->ARR 		// counter period(auto-reload register) set
	// htim->Instance->psc 		// prescaler set

	//cnt = TIM8->CNT;			// encoder count input
	//TIM8->CNT = 0;			// encoder count clear

	//control_flow_latte.input_angle_r64 = (real32_T)0;
	//control_flow_latte.input_velo_r64 = (real32_T)1000;

	control_flow.inputs->Input2.encoder_u16 = (uint16_T)TIM8->CNT;
	TIM8->CNT = 0;
	control_flow.inputs->Input2.motor_dir_u16 = (uint16_T)HAL_GPIO_ReadPin(motor_dir_GPIO_Port, motor_dir_Pin);
	//control_flow.inputs->Input2.accel_u32 = (uint32_T)TIM10->CCR1;

	control_flow_step(&control_flow);

	HAL_GPIO_WritePin(blinker_left_GPIO_Port, blinker_left_Pin, control_flow.outputs->Output1.blinker_left_u8);
	HAL_GPIO_WritePin(blinker_right_GPIO_Port, blinker_right_Pin, control_flow.outputs->Output1.blinker_right_u8);
	HAL_GPIO_WritePin(break_light_GPIO_Port, break_light_Pin, control_flow.outputs->Output1.start_stop_u8);
	
	HAL_GPIO_WritePin(motor_dir_GPIO_Port, motor_dir_Pin, control_flow.outputs->Output1.motor_dir_u8);	//motor _direction
	TIM10->CCR1 = (uint32_t)control_flow.outputs->Output1.motor_val_u32;
	TIM11->CCR1 = (uint32_t)control_flow.outputs->Output1.servo_val_u32;
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM9)
	{
		timer9_motor_ISR();
	}

	if(htim->Instance == TIM7)
	{
		timer7_ISR(htim);
	}
}

