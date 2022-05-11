/*
 * motor.c
 *
 *  Created on: 2022. 3. 10.
 *      Author: kimjs
 */
#include "motor.h"


///////////////////////////////////////////////    motor information   ///////////////////////////////////////////////////

//#define WHEEL_RADIUS			36
//#define Gear_Ratio 			3.35
//#define M_PI					3.141592653589

//#define SAMPLE_FRQ			0.00025			//250us
#define SAMPLE_FRQ_MS			0.25

//PULSE_TO_D = (WHEEL_RADIUS * M_PI) / (encoder_pulse * 4) / geer_ratio

// (65 * M_PI) / 52 / 12 * 1.39534884 = 16.744186  ->  0.2345286188881472
//#define PULSE_TO_D				0.234528618888	1472
//#define PULSE_TO_V  				938.1144755525	888

//#define PULSE_TO_D				0.234528618888
//#define PULSE_TO_V  				938.1144755525



//(36 * M_PI) / 2048 / 3.35
#define PULSE_TO_D				0.016484569660

//PULSE_TO_V = (WHEEL_RADIUS * M_PI) / (encoder_pulse * 4) / geer_ratio / SAMPLE_FRQ
//(36 * M_PI) / 2048 / 3.35 / 0.00025

#define PULSE_TO_V  			65.93827864344



////////////////////////////////////////////       PID information       ///////////////////////////////////////////////////

//#define MAX_PID_OUT				8950.0
//#define MIN_PID_OUT				-8950.0

#define MAX_PID_OUT				5012.0
#define MIN_PID_OUT				-5012.0


#define PWM_CONVERT				0.3333333333333

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//떨어 뜨릴 값이 0.2 까지 이므로 1.5에서 0.2의 차이는 1.3 이다.
//따라서 간 거리가 200이 될때까지 1.3을 떨어뜨려야 하므로 X * 200 = 1.3 이된다.
//#define DOWN_KP				( float32 )( 0.00725 )
#define	DOWN_KP				( float )( 0.007 )  // 0.1
//#define	DOWN_KP				( float32 )( 0.0065 )  // 0.2

#define DOWN_KD				( float )( 0.005 )	//3.4
//#define DOWN_KD				( float32 )( 0.01 )		//2.4

///////////////////////////////////////////        jerk control            /////////////////////////////////////////////////////


//jerk time.
//T = ( ( 60 * S / x ) ^ 1/3 ) s

//x에 관한 식으로 고치면...
//x = ( ( 60 * S ) / T^3 ) m/s^3

//로봇에 사용하려면 mm/s^3으로 고쳐야 된다.( 거리가 mm단위를 사용하므로... )
//x = ( ( 6 * S ) / ( 2.5 * ( 0.00025 )^2 ) ) mm/s^3
//S는 한 인터럽트 당 거리 * qep value -> fp32tick_distance = int16qep_value * PULSE_TO_D;

//모든 계산을 거치면 9600 * S 가 나오는데 가속도를 작은 값으로 쓰기 위해 시간이 us가 아니라 ms 이므로 9.6 * S 가 된다.

//시작 할 때는 S가 0 이므로 최대 가속도 17일때의 거리 S = 1/2at^2 에 의해 0.5 * 17000 * ( 0.00025 )^2 = 0.00053125 로 강제 처리.

//#define JERK_CONTROL


#ifdef JERK_CONTROL
#define JERK_VALUE			( float )0.96
#define START_JERK_LIMIT	( float )0.00053125
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

float_t	am_g_motor_step = 0.0;

//float32	am_Rmotor_step = 0.0;
//float32	am_Lmotor_step = 0.0;

RT_MODEL_control_flow_T control_flow;
B_control_flow_T control_flow_B;
ExtU_control_flow_T control_flow_IN;
ExtY_control_flow_T control_flow_OUT;
DW_control_flow_T control_flow_DW;

bit_field_flag_t g_Flag;
motor_vari g_motor;

/* motor variable struct initialize func */

void init_motor_variable( motor_vari *pm )
{
	memset( ( void * )pm , 0x00 , sizeof( motor_vari ) );

	pm->fp32kp = 0.6; // 0.8
	pm->fp32ki = 0.00002;
	pm->fp32kd = 0.65; // 0.85 //정지 토크만 만족하면 된다 -> 더 떨어뜨려도 될듯.

	pm->int32accel = 5;

#ifdef JERK_CONTROL
	pm->fp32next_acc = START_JERK_LIMIT;
#endif

	/*test*/
	pm->fp32user_vel = 0;
	g_Flag.start_flag = 1;

	//control_flow_initialize(&control_M, &control_U, &control_Y);

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

#if 0 // motor interrupt

	/* qep value sampling */
	g_motor.u16qep_sample = TIM8->CNT;		// encoder cnt

	/* qep reset */
	TIM8->CNT = 0;							// encoder count clear

	/* qep counter value signed */
	//g_motor.int16qep_value = g_motor.u16qep_sample > 1024 ? ( int16_t )( g_motor.u16qep_sample ) - 2049 : ( int16_t )g_motor.u16qep_sample;
	g_motor.int16qep_value = g_motor.u16qep_sample > 26 ? ( int16_t )( g_motor.u16qep_sample ) - 53 : ( int16_t )g_motor.u16qep_sample;

	/* distance compute */
	g_motor.fp32tick_distance = ( float )g_motor.int16qep_value * ( float )PULSE_TO_D;
	g_motor.fp32distance_sum += g_motor.fp32tick_distance;
	g_motor.fp32err_distance = g_motor.fp32user_distacne - g_motor.fp32distance_sum;


	/* average velocity compute */
	g_motor.fp32current_vel[ 3 ] = g_motor.fp32current_vel[ 2 ];
	g_motor.fp32current_vel[ 2 ] = g_motor.fp32current_vel[ 1 ];
	g_motor.fp32current_vel[ 1 ] = g_motor.fp32current_vel[ 0 ];
	g_motor.fp32current_vel[ 0 ] = ( float )g_motor.int16qep_value * ( float )PULSE_TO_V;
	g_motor.fp32cur_vel_avr = ( g_motor.fp32current_vel[ 0 ] + g_motor.fp32current_vel[ 1 ] + g_motor.fp32current_vel[ 2 ] + g_motor.fp32current_vel[ 3 ] ) * 0.25;


	/* decelation a point of time flag */
	if( g_motor.u16decel_flag ) //if move_to_move or move_end function called
	{
		if( g_motor.fp32decel_distance >= ( float )fabs( ( double )( g_motor.fp32err_distance ) ) ) //가속 할 수 있는 계산된 구간이 지났을 경우.
		{
			/*
			if( g_secinfo[ g_int32mark_cnt ].int32dir & STRAIGHT )
			{
				RED_ON;
				BLUE_OFF;
			}
			*/

	#ifdef JERK_CONTROL  //감속 구간 이므로 가속도를 뒤집는다.
			g_motor.int32accel = -g_motor.int32accel;
			//L_motor.int32accel = -L_motor.int32accel;
	#endif

			g_motor.fp32user_vel = g_motor.fp32decel_vel; //user_vel -> decel_vel

			g_motor.u16decel_flag = 0;

			/* accelation start flag OFF */
			g_Flag.speed_up = 0;
			g_Flag.speed_up_start = 0;

			//g_err.fp32over_dist = 0.0;
		}

	}


	#ifdef JERK_CONTROL

		/* jerk accel & decel compute */
		if( ( float )( g_motor.int32accel ) > g_motor.fp32next_acc )
		{
			g_motor.fp32next_acc += ( JERK_VALUE * g_motor.fp32tick_distance );
			if( ( float )g_motor.int32accel < g_motor.fp32next_acc )
				g_motor.fp32next_acc = ( float )( g_motor.int32accel );
		}
		else if( ( float )( g_motor.int32accel ) < g_motor.fp32next_acc )
		{
			g_motor.fp32next_acc -= ( JERK_VALUE * g_motor.fp32tick_distance );
			if( ( float )g_motor.int32accel > g_motor.fp32next_acc )
				g_motor.fp32next_acc = ( float )( g_motor.int32accel );
		}
		else;
	#else
	g_motor.fp32next_acc = ( float )g_motor.int32accel;

		// R_motor.fp32next_acc = ( float32 )R_motor.int32accel;
		// L_motor.fp32next_acc = ( float32 )L_motor.int32accel;

	#endif

		/* accel & decel compute */
		if( g_motor.fp32user_vel > g_motor.fp32next_vel )
		{
			g_motor.fp32next_vel += ( ( float )fabs( ( double )( g_motor.fp32next_acc ) ) * ( float )SAMPLE_FRQ_MS );
			if( g_motor.fp32user_vel < g_motor.fp32next_vel )
				g_motor.fp32next_vel = g_motor.fp32user_vel;
		}
		else if( g_motor.fp32user_vel < g_motor.fp32next_vel )
		{
			g_motor.fp32next_vel -= ( ( float )fabs( ( double )( g_motor.fp32next_acc ) ) * ( float )SAMPLE_FRQ_MS );
			if( g_motor.fp32user_vel > g_motor.fp32next_vel )
				g_motor.fp32next_vel = g_motor.fp32user_vel;
		}
		else;


		/* motor PID compute */
		g_motor.fp32err_vel_sum -= g_motor.fp32err_vel[ 3 ];
		g_motor.fp32err_vel[ 3 ] = g_motor.fp32err_vel[ 2 ];
		g_motor.fp32err_vel[ 2 ] = g_motor.fp32err_vel[ 1 ];
		g_motor.fp32err_vel[ 1 ] = g_motor.fp32err_vel[ 0 ];
		//g_motor.fp32err_vel[ 0 ] = ( g_motor.fp32next_vel * g_fp32right_handle ) - g_motor.fp32cur_vel_avr;	// g_fp32right_handle -> x
		g_motor.fp32err_vel[ 0 ] = ( g_motor.fp32next_vel ) - g_motor.fp32cur_vel_avr;
		g_motor.fp32err_vel_sum += g_motor.fp32err_vel[ 0 ];

		g_motor.fp32proportion_val = g_motor.fp32kp * g_motor.fp32err_vel[ 0 ];
		g_motor.fp32integral_val = g_motor.fp32ki * g_motor.fp32err_vel_sum;
		g_motor.fp32differential_val = g_motor.fp32kd * ( ( g_motor.fp32err_vel[ 0 ] - g_motor.fp32err_vel[ 3 ] ) + ( ( float )3.0 * ( g_motor.fp32err_vel[ 1 ] - g_motor.fp32err_vel[ 2 ] ) ) );
		g_motor.fp32PID_output += g_motor.fp32proportion_val + g_motor.fp32integral_val + g_motor.fp32differential_val;


		//pPwmRegs->TBPRD = 3000;// 1/(6.67ns*3000) = 50Khz(1  20us )//

		//PA12_MOTOR_DIR_GPIO_Port->BSRR = PA12_MOTOR_DIR_Pin;  // gpio set;
		//PA12_MOTOR_DIR_GPIO_Port->BSRR = (uint32_t)PA12_MOTOR_DIR_Pin << 16U; 	// gpio resetֱ

		if( g_Flag.start_flag ) //실제 주행일 경우
		{
			/* PID -> PWM */
			if( g_motor.fp32PID_output > 0.0 )
			{
				if( g_motor.fp32PID_output > MAX_PID_OUT )
					g_motor.fp32PID_output = MAX_PID_OUT;



				PB7_MOTOR_DIR_GPIO_Port->BSRR = PB7_MOTOR_DIR_Pin;  // gpio set;

				TIM10->CCR1 = ( uint32_t )( g_motor.fp32PID_output * PWM_CONVERT );


			}
			else
			{
				if( g_motor.fp32PID_output < MIN_PID_OUT )
					g_motor.fp32PID_output = MIN_PID_OUT;


				PB7_MOTOR_DIR_GPIO_Port->BSRR = (uint32_t)PB7_MOTOR_DIR_Pin << 16U; 	// gpio reset

				TIM10->CCR1 = ( uint32_t )( g_motor.fp32PID_output * PWM_CONVERT * (-1) );

			}

		}
		else //모터 상 풀기
		{
			//HAL_GPIO_WritePin(PB7_MOTOR_DIR_GPIO_Port, PB7_MOTOR_DIR_Pin, GPIO_PIN_RESET);

			//GpioDataRegs.GPBSET.bit.GPIO48 = 1; // left
			//GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1; // right
			TIM10->CCR1 = 0;
			//LeftPwmRegs.CMPA.half.CMPA = 0;  //모터 정지
			//LeftPwmRegs.CMPB = 0;
		}
#endif

	//HAL_GPIO_TogglePin(PA12_MOTOR_DIR_GPIO_Port, PA12_MOTOR_DIR_Pin);

}
