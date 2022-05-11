/*
 * struct.h
 *
 *  Created on: Mar 10, 2022
 *      Author: kimjs
 */

#ifndef __STRUCT_H__
#define __STRUCT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef volatile struct bit_field_flag
{

	uint16_t motor_start:1;
	uint16_t motor_ISR_flag:1;
	uint16_t move_state:1;
	uint16_t start_flag:1;
	uint16_t cross_flag:1;
	uint16_t speed_up_flag:1;
	uint16_t debug_flag:1;
	uint16_t fast_flag:1;
	uint16_t lineout_flag:1;
	uint16_t err:1;
	uint16_t speed_up_start:1;
	uint16_t speed_up:1;
	uint16_t straight_run:1;


}bit_field_flag_t;


typedef volatile struct motor_variable
{
	uint16_t	u16qep_sample;
	uint16_t	u16decel_flag;

	int16_t		int16qep_value;
	int32_t		int32accel;

	float		fp32next_acc;

	float		fp32distance_sum;
	float		fp32err_distance;
	float		fp32user_distacne;

	float		fp32user_vel;
	float		fp32next_vel;

	float		fp32current_vel[ 4 ];
	float		fp32cur_vel_avr;

	float		fp32err_vel[ 4 ];
	float		fp32err_vel_sum;

	float		fp32PID_output;
	float		fp32kp;
	float		fp32ki;
	float		fp32kd;
	float		fp32proportion_val;
	float		fp32differential_val;
	float		fp32integral_val;

	float		fp32decel_distance;
	float		fp32decel_vel;

	float		fp32gone_distance;
	float		fp32tick_distance;

}motor_vari;

typedef struct {
	uint8_t 			rsvd:5;
	uint8_t 			blinker:1;
	uint8_t 			turn_onoff:1;
	uint8_t 			move_onoff:1;
} st_flag;

union un_flag {
	uint8_t 			all;
	st_flag 			bit;
};

typedef struct {
	uint8_t 			start;
	uint8_t 			mode;
	union 	un_flag 	flag;
	uint8_t 			angle;
	uint8_t 			stop;
} st_protocol;


//__STRUCT_EXT__ motor_vari	g_motor;
//__STRUCT_EXT__ motor_vari	R_motor;
//__STRUCT_EXT__ motor_vari	L_motor;


//__STRUCT_EXT__


#ifdef __cplusplus
}
#endif

#endif /* __STRUCT_H__ */
