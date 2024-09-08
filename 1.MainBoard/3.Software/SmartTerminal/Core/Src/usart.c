/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include <stdio.h>

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart6;

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
/* USART6 init function */

void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

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

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspInit 0 */

  /* USER CODE END USART6_MspInit 0 */
    /* USART6 clock enable */
    __HAL_RCC_USART6_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**USART6 GPIO Configuration
    PC6     ------> USART6_TX
    PC7     ------> USART6_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* USART6 interrupt Init */
    HAL_NVIC_SetPriority(USART6_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART6_IRQn);
  /* USER CODE BEGIN USART6_MspInit 1 */

  /* USER CODE END USART6_MspInit 1 */
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

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART6)
  {
  /* USER CODE BEGIN USART6_MspDeInit 0 */

  /* USER CODE END USART6_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART6_CLK_DISABLE();

    /**USART6 GPIO Configuration
    PC6     ------> USART6_TX
    PC7     ------> USART6_RX
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6|GPIO_PIN_7);

    /* USART6 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART6_IRQn);
  /* USER CODE BEGIN USART6_MspDeInit 1 */

  /* USER CODE END USART6_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */



#pragma import(__use_no_semihosting)
struct __FILE
{
	int a;
};
FILE __stdout;
FILE __stdin;

void _sys_exit(int x)
{
	
}

/*****************************************************
*function: д�ַ��ļ�����
*param1: ������ֹJ
*param2: �ļ�ָ��
*return: ����ַ���ASCII�m
******************************************************/
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 10);
	return ch;
}
/*****************************************************
*function: ���ַ��ļ�����
*param1: �ļ�ָ��
*return: ��ȡ�ַ���ASCII�m
******************************************************/
int fgetc(FILE *f)
{
	uint8_t ch = 0;
	HAL_UART_Receive(&huart1, (uint8_t*)&ch, 1, 10);
	return (int)ch;
}

/***************************************/

static uint8_t rx_data = 0;            //���յ��ֹJ
static uint8_t rx_len = 0;             //���յĳ���
static uint8_t usart_rx_buf[200] = {0};//������յ�����

//����HAL����պ���,�ȴ���������
void USART_StartRx(void)
{
	HAL_UART_Receive_IT(&huart6, (uint8_t*)&rx_data, 1);
}

void EnableUART6IRQ(void)
{
    HAL_NVIC_SetPriority(USART6_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(USART6_IRQn);
    
    __HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE); 
}

enum AT_STATUS {
	INIT_STATUS,
	LEN_STATUS,
	DATA_STATUS
};

static enum AT_STATUS g_status = INIT_STATUS;

#define DATA_BUF_LEN 512
static char g_ESP8266DataBuff[DATA_BUF_LEN];
static int g_DataBuffIndex = 0;
static int g_DataLen = 0;

#define PACKET_BUF_LEN 512
static char g_ESP8266PacketBuff[PACKET_BUF_LEN];
static int g_PacketR = 0;
static int g_PacketW = 0;

static void ESP8266PutPacketToBuf(char *buf, int len)
{
	int i;
	int w;
	
	for (i = 0; i < len; i++)
	{
		w = (g_PacketW+1) & 511;
		if (w != g_PacketR) /* δ�� */
		{
			g_ESP8266PacketBuff[g_PacketW] = buf[i];
			g_PacketW = w;
		}
		else
		{
			return;
		}
	}
}


int ESP8266ReadPacket(unsigned char *buf, int len)
{
	int i;

	for (i = 0; i < len; i++)
	{
		if (g_PacketR != g_PacketW)
		{
			buf[i] = g_ESP8266PacketBuff[g_PacketR];
			g_PacketR = (g_PacketR+1) & 511;
		}
		else
		{
			return i;
		}
	}

	return i;
}

static void StoreRawDataUART2(char c)
{
	if (rx_len < 200)
	{
		usart_rx_buf[rx_len++]= c;
	}
}


/**********************************************************************
 * �������ƣ� ESP8266DataProcessCallback
 * ���������� �����������ݻص�����: �õ���������,����ΪInputEvent
 * ��������� c-��������
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2021/10/16	     V1.0	  Τ��ɽ	      ����
 ***********************************************************************/ 

void ESP8266DataProcessCallback(char c)
{	
	/* �������� */
	char *buf = g_ESP8266DataBuff;
	int i = g_DataBuffIndex;
	int m;
	static unsigned int time_get_data = 0;
	extern unsigned int xTaskGetTickCount( void );

	StoreRawDataUART2(c);
	
#if 1
	if (time_get_data && (xTaskGetTickCount() - time_get_data > 1000))
	{
		/* ��ʱ */
		i = g_DataBuffIndex = 0;
		g_status = INIT_STATUS;
		time_get_data = 0;
	}
#endif
	buf[i] = c;
	g_DataBuffIndex++;

	/* ����: +IPD,n:xxxxxxxxxx */
	switch (g_status)
	{
		case INIT_STATUS:
		{
			if (buf[0] != '+')
			{
				g_DataBuffIndex = 0;
			}			
			else if (i == 4)
			{
				if (strncmp(buf, "+IPD,", 5) == 0)
				{
					g_status = LEN_STATUS;
				}
				g_DataBuffIndex = 0;
			}
			break;
		}

		case LEN_STATUS:
		{
			if (buf[i] == ':')
			{
				/* ���㳤�� */
				for (m = 0; m < i; m++)
				{
					g_DataLen = g_DataLen * 10 + buf[m] - '0';
				}
				g_status = DATA_STATUS;
				g_DataBuffIndex = 0;

				time_get_data = xTaskGetTickCount();
			}
			else if (i >= 9)  /* ESP8266����buffer��С��2920,  4λ��: +IPD,YYYY:xxxxxx */
			{
				/* ��û�н��յ�':' */
				g_status = INIT_STATUS;
				g_DataBuffIndex = 0;
			}
			break;
		}

		case DATA_STATUS:
		{
			if (g_DataBuffIndex == g_DataLen)
			{
				/* ���������� */
				ESP8266PutPacketToBuf(buf, g_DataLen);
				if (g_DataLen != 2)
				{
					printf("ISR Get Data: ");
					for (i = 0; i < g_DataLen; i++)
						printf("%02x ", buf[i]);
					printf("\r\n");
				}

				/* �ָ���ʼ״̬ */
				g_status = INIT_STATUS;
				g_DataBuffIndex = 0;
				g_DataLen = 0;
			}
			break;
		}
		
	}

	
}

//USART�жϻص�����,������յ����ݲ�������һ�ν���

//ע�⣺���жϺ����ѱ����ã�����//
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	usart_rx_buf[rx_len%200]= rx_data;
	rx_len++;
	rx_len = rx_len%200;
	ESP8266DataProcessCallback(rx_data);
	HAL_UART_Receive_IT(&huart6, (uint8_t*)&rx_data, 1);
}

//��ȡ���յ�����
uint8_t USART2_Receive(uint8_t *rx_data)
{
	memcpy(rx_data, (uint8_t*)usart_rx_buf, rx_len);
	return rx_len;
}

//UASRT2��?����
void USART2_Transmit(uint8_t *pdata, uint16_t len, uint16_t tt)
{
    HAL_UART_Transmit(&huart6, pdata, len, tt);
}

//������յ�����
void USART2_ClearBuf(void)
{
    rx_len = 0;
    memset((uint8_t*)usart_rx_buf, 0, 200);
}




//��ESP8266��?����
uint8_t ESP8266_SendCommand(char *cmd, char *reply, uint16_t timeout)
{
	//1.���洫���ָ��
	char buf[256] = {0};
	strcat(buf, cmd); //strcpy 
	
	//2.����ATָ��(��ӻ���)
	if (strstr(buf, "\r\n") == 0)
	{
		strcat(buf, "\r\n");
	}
	
	//3.����ǰ����յ�buf
	USART2_ClearBuf();   
	
	//4.��?����
	USART2_Transmit((uint8_t *)buf, strlen(buf), 500);
	
	//5.��������
	memset(buf, 0, 256); //buf���
	while(timeout != 0)  //��ʱ����
	{
		if (USART2_Receive((uint8_t *)buf)) //��������
		{
			//����ᖨ
			if (strstr(buf, reply)) 
			{
				printf("%s Send ok!\r\n", cmd);//��?����
				return 0;
			}
		}	
		timeout--;
		HAL_Delay(1);
	}

	printf("%s Send error!\r\n", cmd);//��?ʧ��
	return 1;
}


//��ESP8266��������
uint8_t ESP8266_SendRawData(unsigned char *data, int len, char *reply, uint16_t timeout)
{
	char buf[256] = {0};
	
	//3.����ǰ����յ�buf
	USART2_ClearBuf();   
	
	//4.��������
	USART2_Transmit((uint8_t *)data, len, 500);
	
	//5.��������
	memset(buf, 0, 256); //buf���
	while(timeout != 0)  //��ʱ����
	{
		if (USART2_Receive((uint8_t *)buf)) //��������
		{
			// �жϷ��ص��ַ����Ƿ����ڴ����ַ�
			if (strstr(buf, reply)) 
			{
				return 0;
			}
			else
			{
				timeout--;
				HAL_Delay(1);
			}
		}	
	}

	return 1;
}



//��ȡESP8266��IP
uint8_t ESP8266_GetIP(void)
{
	uint16_t timeout = 500;
	
	//1.׼����?��ָ�� AT+CIFSR
	char buf[256] = {0};
	strcat(buf, "AT+CIFSR\r\n");
	
	//2.����ǰ����յ�buf
	USART2_ClearBuf();   
	
	//4.��?����
	USART2_Transmit((uint8_t *)buf, strlen(buf), 500);
	
	//5.��������
	memset(buf, 0, 256); //buf���
	
	while(timeout != 0)  //��ʱ����
	{
		if (USART2_Receive((uint8_t *)buf)) //��������
		{
			//����ᖨ
			if (strstr(buf, "OK")) 
			{
				printf("%s", buf); //��ӡIP
				//Show_IP(buf);
				return 0;
			}
			else
			{
				timeout--;
				HAL_Delay(1);
			}
		}	
	}
	
	printf("Get IP Failed! \r\n"); //��ȡʧ��
	return 1;
}


//ESP8266��������
uint8_t ESP8266_Send_Data(char *data)
{
	//1.׼����?��ָ�� AT+CIPSEND=len
	char buf[256] = {0};
	uint8_t len = strlen(data);
	sprintf(buf, "AT+CIPSEND=%d\r\n", len); //�Ѹ�ʽ��������д���ַ���
	if (ESP8266_SendCommand(buf, "OK", 500) == 0) //��?ָ��
	{
		ESP8266_SendCommand(data, "SEND OK", 1000); //��?����
		return 0;
	}
	return 1;
}

//ESP8266��������
int ESP8266_Send_Packet(unsigned char *data, int len)
{
	//1.׼����?��ָ�� AT+CIPSEND=len
	char buf[256] = {0};
	sprintf(buf, "AT+CIPSEND=%d\r\n", len); //�Ѹ�ʽ��������д���ַ���
	if (ESP8266_SendCommand(buf, "OK", 500) == 0) //��?ָ��
	{
		ESP8266_SendRawData(data, len, "SEND OK", 1000); //��?����
		return len;
	}
	return 0;
}

#if 0
//��ӹ���1��֧���ֶ�����WIFI�˺ź��ܳm
void Key_Update_WIFI(char *cmd, uint16_t d_time)
{
	char ssid_buf[20] = "Programmers";
	char pwd_buf[20] = "100asktech";
	char wifi_cmd[50] = "AT+CWJAP=\"";
	
	char ch = 0;
	char len = 0;
	
	//1.���õȴ�ʱ��
	HAL_Delay(d_time);
	
	//2.�ж��Ƿ��а���
	if (Key_GetFlag()) 
	{
		//2.1��ȡWIFI�˺�
		printf("Please enter SSID:");
		
		memset(ssid_buf, 0, 20);
		while(ch != '\r') //�ж��Ƿ������� 
		{
			ch = getchar();    //��ȡ������ֹJ
			if(ch > ' ')
			{
				printf("%c",ch);
				ssid_buf[len] = ch;//��������
				len++;
			}
		}	
		
		//2.2��ȡWIFI����
		ch = 0;
		len = 0;
		printf("\r\nPlease enter Password:");

		memset(pwd_buf, 0, 20);
		while(ch != '\r') //�ж��Ƿ������� 
		{
			ch = getchar();    //��ȡ������ֹJ
			if(ch > ' ')
			{
				printf("%c",ch);
				pwd_buf[len] = ch;//��������
				len++;
			}
		}	
	}
	printf("\r\n");
	
	//3.���ָ�� ("AT+CWJAP=\"NETGEAR\",\"100ask.cq\"") 
	strcat(wifi_cmd, ssid_buf);
	strcat(wifi_cmd, "\",\"");
	strcat(wifi_cmd, pwd_buf);
	strcat(wifi_cmd, "\"");

	memcpy(cmd, wifi_cmd, strlen(wifi_cmd));
}

#endif

//��ӹ���2���Ż�չʾESP8266 IP
/*
"192.168.50.170"
+CIFSR:STAMAC,"ac:0b:fb:c7:df:30"
*/
#if 0
void Show_IP(char *ip)
{
	char buf[256] = {0};
	char ip_buf[256] = {0};
	uint8_t len = 0;
	
	char *ip_offset;
	ip_offset = strstr(ip, "STAIP")+6;
	
	memcpy(buf, ip_offset, strlen(ip_offset));
	while(buf[len] != '\r')  
	{
		ip_buf[len] = buf[len];
		len++;
	}
	
	printf("ESP8266 IP: %s\r\n", ip_buf);
}
#endif
/* USER CODE END 1 */
