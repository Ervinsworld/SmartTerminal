/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "driver_motorComm.h"
//#include "driver_mpu6050.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "driver_led.h"
#include "driver_joystick.h"
#include "driver_oled.h"
#include "UI.h"
#include "page.h"
#include "joystick.h"
#include "Motor.h"
/** User FreeRTOS  **/
#include "queue.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core
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
/* USER CODE BEGIN Variables */
TaskHandle_t MotorPidTaskHandle;

//QueueHandle_t MotorInfQueueHandle;

QueueHandle_t TargetAngleQueueHandle;
QueueHandle_t AngleDiffQueueHandle;
//QueueHandle_t MotorRawQueueHandle;

//QueueHandle_t ButtonQueueHandle;

//SemaphoreHandle_t ActionSemaphore;
EventGroupHandle_t UIResponseEvent;//摇杆事件对应的event（上下左右按下）
EventGroupHandle_t UIActionEvent;//UI子页面和电机动作交互的事件

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	OLED_Init();//oled初始化
	JoyStickInit();//摇杆初始化
	MotorCommInit();//电机通讯初始化
	//MPU_Init();//MPU6050初始化
    //mpu_dmp_init();		//dmp初始化
	printf("Everything is Inited！\r\n");
	
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
	//ActionSemaphore = xSemaphoreCreateBinary();
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */

	//MotorRawQueueHandle = xQueueCreate(1, sizeof(uint8_t)*8);//接受CAN通信传来的原始电机数据帧
	TargetAngleQueueHandle = xQueueCreate(1, sizeof(float));//UI页面想要设置angle的目标值mailbox
	AngleDiffQueueHandle = xQueueCreate(1, sizeof(float));//用来消除刚进入子页面时的angle差异（消除进入子页面时的电机快速转动）
	
	//ButtonQueueHandle = xQueueCreate(1, sizeof(int));
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  

  xTaskCreate(MotorPid_Task, "MotorPidTask", 128, NULL, osPriorityNormal+2, &MotorPidTaskHandle);
  xTaskCreate(Joystick_Task, "JoystickTask", 128, NULL, osPriorityNormal+1, NULL);
  xTaskCreate(UI_Task, "UITask", 256, NULL, osPriorityNormal+1, NULL);
  xTaskCreate(UIAction_Task, "UIAction_Task", 256, NULL, osPriorityNormal+1, NULL);
  
  /*test_thread*/
  //xTaskCreate(AnglePrint_Task, "test", 128, NULL, osPriorityNormal+1, NULL);
 
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  
  //UI换页响应事件组使用低五位：0位：右；1位：左，2位：下，3位：上，4位：按钮
  UIResponseEvent = xEventGroupCreate();
  //UI动作事件只用最低位
  UIActionEvent = xEventGroupCreate(); 
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	
  /* Infinite loop */
  for(;;)
  {
	vTaskDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

