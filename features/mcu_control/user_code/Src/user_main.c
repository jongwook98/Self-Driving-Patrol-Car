
#include "tim.h"
#include "user_main.h"

void main_init(void)
{
	HAL_TIM_Base_Start_IT(&htim9); 		// APB2 TIMER IT(168)
	HAL_TIM_Base_Start_IT(&htim7);    	// APB1 TIMER IT(84)
	
	HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1);
	
	HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_ALL);
	HAL_TIM_Base_Start_IT(&htim1);
	 
	control_flow.blockIO = &control_flow_B;
	control_flow.inputs = &control_flow_IN;
	control_flow.outputs = &control_flow_OUT;
	control_flow.dwork = &control_flow_DW;
	
	control_flow_initialize(&control_flow);
	
	control_flow.inputs->Input1.input_angle_r32 = (real32_T)0;
	control_flow.inputs->Input1.input_velo_r32 = (real32_T)0;

	receive_uart_start_it();

//--------------------------------------------------------------------------------//

	TxPrintf("\n-----\nRESET\n-----\n");
}

void main_while(void)
{
	if(st_ptcl.start == 0x55 && st_ptcl.stop == 0x77) {
		TxPrintf("%x %x %x %x %x\n", st_ptcl.start, st_ptcl.mode, 
						st_ptcl.flag.all, st_ptcl.angle, st_ptcl.stop);
	
		memset((void*)&st_ptcl, 0, sizeof(st_protocol));
	}
	
	//TxPrintf("|sample : %d|value : %d|\n", g_motor.u16qep_sample, g_motor.int16qep_value );
	//TxPrintf("|sample : %d|value : %d|\n", g_motor.u16qep_sample, HAL_GPIO_ReadPin(PB7_MOTOR_DIR_GPIO_Port, PB7_MOTOR_DIR_Pin) );
	//TxPrintf("flag1 : %u |flag2 : %u pid_out : %f |\n", (PD7_LED_GPIO_Port->IDR & PD7_LED_Pin),HAL_GPIO_ReadPin(PD7_LED_GPIO_Port, PD7_LED_Pin), g_motor.fp32PID_output );
	
	
	//PA12_MOTOR_DIR_GPIO_Port->BSRR = PA12_MOTOR_DIR_Pin;  // gpio set;
	//PA12_MOTOR_DIR_GPIO_Port->BSRR = (uint32_t)PA12_MOTOR_DIR_Pin << 16U;	// gpio reset
	
	/*
	TIM11->CCR1 = (pwm_input++);
	
	if(pwm_input == 3000)
	{
		TxPrintf("pwm reset\n");
		pwm_input = 0;
	}
	*/
#if 0
	TxPrintf("servo : %d | motor : %f | motor : %f | dir : %d | encoder : %f\n", TIM8->CNT, control_flow.inputs->Input1.input_velo_r32,//TIM10->CCR1,
	control_flow.blockIO->PID, control_flow.outputs->Output1.motor_dir_u8, control_flow.blockIO->sf_velo_adjust.encoder_velo);
	//HAL_GPIO_ReadPin(PB7_MOTOR_DIR_GPIO_Port, PB7_MOTOR_DIR_Pin), TIM8->CNT);
#endif
}

