/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <user_main.h>

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define receive__led_Pin GPIO_PIN_15
#define receive__led_GPIO_Port GPIOA
#define break_light_Pin GPIO_PIN_0
#define break_light_GPIO_Port GPIOD
#define blinker_right_Pin GPIO_PIN_1
#define blinker_right_GPIO_Port GPIOD
#define blinker_left_Pin GPIO_PIN_2
#define blinker_left_GPIO_Port GPIOD
#define PD7_LED_Pin GPIO_PIN_7
#define PD7_LED_GPIO_Port GPIOD
#define transmit_led_Pin GPIO_PIN_4
#define transmit_led_GPIO_Port GPIOB
#define motor_dir_Pin GPIO_PIN_0
#define motor_dir_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */



/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
