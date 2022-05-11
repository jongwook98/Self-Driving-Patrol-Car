/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "motor.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

	//uint16_t pwm_input = 0;

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */


  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_TIM7_Init();
  MX_TIM8_Init();
  MX_TIM9_Init();
  MX_TIM10_Init();
  MX_USART1_UART_Init();
  MX_TIM11_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  // HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

  HAL_TIM_Base_Start_IT(&htim9);	// APB2 TIMER IT(168)
  HAL_TIM_Base_Start_IT(&htim7);	// APB1 TIMER IT(84)

  HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);

  HAL_TIM_PWM_Start(&htim11, TIM_CHANNEL_1);

  HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_ALL);

  init_motor_variable(&g_motor);

  HAL_TIM_Base_Start_IT(&htim1);

  control_flow.blockIO = &control_flow_B;
  control_flow.inputs = &control_flow_IN;
  control_flow.outputs = &control_flow_OUT;
  control_flow.dwork = &control_flow_DW;

  control_flow_initialize(&control_flow);

  control_flow.inputs->Input1.input_angle_r64 = (real32_T)0;
  control_flow.inputs->Input1.input_velo_r64 = (real32_T)0;

  Receive_DMA();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  TxPrintf("\n-----\nRESET\n-----\n");

  while (1)
  {

	  //TxPrintf("|sample : %d|value : %d|\n", g_motor.u16qep_sample, g_motor.int16qep_value );
	  //TxPrintf("|sample : %d|value : %d|\n", g_motor.u16qep_sample, HAL_GPIO_ReadPin(PB7_MOTOR_DIR_GPIO_Port, PB7_MOTOR_DIR_Pin) );
	  //TxPrintf("flag1 : %u |flag2 : %u pid_out : %f |\n", (PD7_LED_GPIO_Port->IDR & PD7_LED_Pin),HAL_GPIO_ReadPin(PD7_LED_GPIO_Port, PD7_LED_Pin), g_motor.fp32PID_output );


	  //PA12_MOTOR_DIR_GPIO_Port->BSRR = PA12_MOTOR_DIR_Pin;  // gpio set;
	  //PA12_MOTOR_DIR_GPIO_Port->BSRR = (uint32_t)PA12_MOTOR_DIR_Pin << 16U; 	// gpio reset

/*
	  TIM11->CCR1 = (pwm_input++);

	  if(pwm_input == 3000)
	  {
		  TxPrintf("pwm reset\n");
		  pwm_input = 0;
	  }
*/

	//TxPrintf("servo : %d | motor : %f | motor : %f | dir : %d | encoder : %f\n", TIM8->CNT, control_flow.inputs->Input1.input_velo_r64,//TIM10->CCR1,
	//	control_flow.blockIO->PID, control_flow.outputs->Output1.motor_dir_u8, control_flow.blockIO->sf_velo_adjust.encoder_velo);
		//HAL_GPIO_ReadPin(PB7_MOTOR_DIR_GPIO_Port, PB7_MOTOR_DIR_Pin), TIM8->CNT);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
