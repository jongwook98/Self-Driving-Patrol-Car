/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "tim.h"
#include "motor.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define TX_LED_ON	HAL_GPIO_WritePin(transmit_led_GPIO_Port,transmit_led_Pin,RESET)
#define TX_LED_OFF	HAL_GPIO_WritePin(transmit_led_GPIO_Port,transmit_led_Pin,SET)
#define RX_LED_ON	HAL_GPIO_WritePin(receive__led_GPIO_Port,receive__led_Pin,RESET)
#define RX_LED_OFF	HAL_GPIO_WritePin(receive__led_GPIO_Port,receive__led_Pin,SET)

#define BUF_SIZE 128
static uint8_t buf;
static uint8_t rxBuffer[BUF_SIZE];
static uint8_t txBuffer[BUF_SIZE];

static uint8_t buf_latte;
static uint8_t rx_latte[BUF_SIZE];
//static uint8_t tx_latte[BUF_SIZE];

st_protocol st_ptcl;

uint8_t txERR[] = "Something's wrong\r\n";
uint32_t tim_cnt = 0;

char* CR = "\r";

void USARTx_TxString(volatile UART_HandleTypeDef *USARTx, uint8_t *buffer ,char *Str)
{
	uint16_t strCnt = 0;
	while(*Str)
	{
		if(*Str == '\n') {
			strcat((char*)buffer, CR);
			strCnt++;
		}
		strncat((char*)buffer, Str++, 1);
		strCnt++;
	}
	if(strCnt == 0)
		return;

	//HAL_UART_Transmit(&huart1, txBuffer, ++strCnt, 15);
	HAL_UART_Transmit((UART_HandleTypeDef*)USARTx, (uint8_t*)buffer, ++strCnt, 15);

	memset((void*)buffer, 0x00, sizeof(uint8_t)*BUF_SIZE);
}


void TxPrintf(char *Form, ... )
{
	TX_LED_ON;

	tim_cnt = htim1.Instance->CNT;
	static char Buff[BUF_SIZE];
	va_list ArgPtr;
	va_start(ArgPtr,Form);
	vsprintf(Buff, Form, ArgPtr);
	va_end(ArgPtr);
	USARTx_TxString(&huart1, txBuffer, Buff);
	tim_cnt -= htim1.Instance->CNT;

	TX_LED_OFF;
}

void RxBuffer(uint8_t *buffer, uint8_t *_buf)
{
	if(*_buf == '\r' || *_buf == '\n')
		memset((void*)buffer, 0x00, sizeof(uint8_t)*BUF_SIZE);
	else
		strcat((char*)buffer, (char*)_buf);
}

void linux_protocol(uint8_t *buffer, uint8_t *_buf)
{
	strcat((char*)buffer, (char*)_buf);

	//if(*(buffer + 4) != '\0') {
	if(*_buf == 'w' || *(buffer + 4) != '\0') {
		st_protocol curr = {0, 0, {0}, 0, 0};
		sscanf((const char*)buffer,"%c%c%c%c%c", &curr.start,
			&curr.mode, &curr.flag.all, &curr.angle, &curr.stop);

		if(curr.start != 0x55 || curr.stop != 0x77)
			TxPrintf("protocol error\n");
		else {
			//st_ptcl.start = curr.start;
			//st_ptcl.mode = curr.mode;
			st_ptcl.flag.all = curr.flag.all;
			st_ptcl.angle = curr.angle;
			//st_ptcl.stop = curr.stop;

			TxPrintf("%x %x %x %x %x\n", curr.start,
			curr.mode, st_ptcl.flag.all, st_ptcl.angle, curr.stop);

			control_flow.inputs->Input1.input_angle_r64 = (real_T)st_ptcl.angle - 90.0;
			control_flow.inputs->flag.blinker 	= st_ptcl.flag.bit.blinker;
			control_flow.inputs->flag.turn_move = st_ptcl.flag.bit.turn_onoff;
			control_flow.inputs->flag.move 		= st_ptcl.flag.bit.move_onoff;
		}
		//TxPrintf("%x %x %x %x %x\n", st_ptcl.start,
		//	st_ptcl.mode, st_ptcl.flag, st_ptcl.angle, st_ptcl.stop);

		memset((void*)buffer, 0x00, sizeof(uint8_t)*BUF_SIZE);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	RX_LED_ON;

	if(huart->Instance == USART1) {

		RxBuffer(rxBuffer, &buf);
		//linux_protocol(rxBuffer, &buf);


		switch(buf)
		{
		case 'q':
			control_flow.inputs->Input1.input_velo_r64 += 100;
			//control_flow_latte.input_angle_r64 += (float)10.0;
			break;
		case 'a':
			control_flow.inputs->Input1.input_velo_r64 -= 100;
			//control_flow_latte.input_angle_r64 -= (float)10.0;
			break;
		case 'w':
			control_flow.inputs->Input1.input_angle_r64 += (float)10.0;
			break;
		case 's':
			control_flow.inputs->Input1.input_angle_r64 -= (float)10.0;
			break;
		case 'e':
			if(control_flow.inputs->flag.blinker == 3)
				control_flow.inputs->flag.blinker = 0;
			else
				control_flow.inputs->flag.blinker++;
			break;
		case 'r':
			if(control_flow.inputs->flag.move == 2)
				control_flow.inputs->flag.move = 0;
			else
				control_flow.inputs->flag.move++;
			break;
		}

		HAL_UART_Receive_IT(&huart1, &buf, 1);
	}

	if(huart->Instance == USART2) {
		linux_protocol(rx_latte, &buf_latte);
		HAL_UART_Receive_IT(&huart2, &buf_latte, 1);
	}
/*
	else if(huart->Instance == USART2) {
		if(sizeof(rx_latte) == 5) {
			uint8_t start_b = 0, mode_b = 0, flag_b = 0, angle_b = 0, stop_b = 0;
			sscanf((const char*)rx_latte,"%c%c%c%c%c",&start_b, &mode_b, &flag_b, &angle_b, &stop_b);
			TxPrintf("%x %x %x %x %x\n", start_b, mode_b, flag_b, angle_b, stop_b);
			memset((void*)rx_latte, 0x00, sizeof(uint8_t)*BUF_SIZE);
		}
		else
			strcat((char*)rx_latte, (char*)&buf_latte);

		HAL_UART_Receive_IT(&huart2, &buf_latte, 1);
	}
*/
	RX_LED_OFF;
}

void Receive_DMA(void)
{
	HAL_UART_Receive_IT(&huart1, &buf, 1);
	HAL_UART_Receive_IT(&huart2, &buf_latte, 1);
}

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_RTS_CTS;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA0-WKUP     ------> USART2_CTS
    PA1     ------> USART2_RTS
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA0-WKUP     ------> USART2_CTS
    PA1     ------> USART2_RTS
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
