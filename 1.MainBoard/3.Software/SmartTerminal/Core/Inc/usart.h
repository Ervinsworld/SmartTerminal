/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart6;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART6_UART_Init(void);

/* USER CODE BEGIN Prototypes */

void USART_StartRx(void);
void EnableUART6IRQ(void);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
uint8_t USART2_Receive(uint8_t *rx_data);
void USART2_Transmit(uint8_t *pdata, uint16_t len, uint16_t tt);
void USART2_ClearBuf(void);

uint8_t ESP8266_SendCommand(char *cmd, char *reply, uint16_t timeout);
uint8_t ESP8266_GetIP(void);
uint8_t ESP8266_Send_Data(char *data);


//void Key_Update_WIFI(char *cmd, uint16_t d_time);
//void Show_IP(char *ip);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

