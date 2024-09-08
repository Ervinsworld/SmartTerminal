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
#include "driver_led.h"
#include "driver_joystick.h"
#include "driver_oled.h"
#include "UI.h"
#include "page.h"
#include "joystick.h"
#include "Motor.h"
#include "mqttApp.h"
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

QueueHandle_t rawSpeedHandle;
QueueHandle_t rawAngleHandle;
QueueHandle_t realSpeedHandle;
QueueHandle_t realAngleHandle;

//QueueHandle_t MotorRawQueueHandle;

//QueueHandle_t ButtonQueueHandle;

SemaphoreHandle_t MotorPidSemaphore;
SemaphoreHandle_t UIActionSemaphore;//电机动作交互的事件使能信号量

EventGroupHandle_t UIResponseEvent;//摇杆事件对应的event（上下左右按下）
EventGroupHandle_t MqttNotifyEvent;

static signed char pcWriteBuffer[200];
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

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);
void vApplicationIdleHook(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{

}

__weak unsigned long getRunTimeCounterValue(void)
{
return 0;
}
/* USER CODE END 1 */

/* USER CODE BEGIN 2 */
void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
	

#if 0	
	int i;
	//vTaskList(pcWriteBuffer);
	vTaskGetRunTimeStats(pcWriteBuffer);
	for (i = 0; i < 16; i++)
		printf("-");
	printf("\n\r");
	printf("%s\n\r", pcWriteBuffer);
#endif

#if 0
	printf("%d\n\r", xPortGetFreeHeapSize());
		
#endif
}
/* USER CODE END 2 */

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
	printf("Everything is Inited！\r\n");
	
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
	MotorPidSemaphore = xSemaphoreCreateBinary();
	UIActionSemaphore = xSemaphoreCreateBinary();
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */

	rawSpeedHandle = xQueueCreate(1, sizeof(uint32_t));//原始电机速度
	rawAngleHandle = xQueueCreate(1, sizeof(uint32_t));//原始电机角度
	realSpeedHandle = xQueueCreate(1, sizeof(uint32_t));//真实电机速度
	realAngleHandle = xQueueCreate(1, sizeof(uint32_t));//真实电机角度
	TargetAngleQueueHandle = xQueueCreate(1, sizeof(float));//UI页面想要设置angle的目标值mailbox
	AngleDiffQueueHandle = xQueueCreate(1, sizeof(float));//用来消除刚进入子页面时的angle差异（消除进入子页面时的电机快速转动）
	
	//ButtonQueueHandle = xQueueCreate(1, sizeof(int));
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  
  xTaskCreate(MotorPid_Task, "MotorPidTask", 256, NULL, osPriorityNormal+3, &MotorPidTaskHandle);
  xTaskCreate(CalMotorData_Task, "CalMotorData_Task", 128, NULL, osPriorityNormal+3, NULL);
  xTaskCreate(UI_Task, "UITask", 256, NULL, osPriorityNormal+2, NULL);
  xTaskCreate(UIAction_Task, "UIAction_Task", 256, NULL, osPriorityNormal+1, NULL);
  xTaskCreate(Joystick_Task, "JoystickTask", 128, NULL, osPriorityNormal+1, NULL);
  xTaskCreate(MqttTask, "MqttTask", 1024, NULL, osPriorityNormal+1, NULL);
  //xTaskCreate(AnglePrint_Task, "AnglePrintTask", 128, NULL, osPriorityNormal+1, NULL);
  //xTaskCreate(ButtonTest_Task, "ButtonTest_Task", 128, NULL, osPriorityNormal, NULL);

 
  /*test_thread*/
  //xTaskCreate(ButtonTest_Task, "test", 128, NULL, osPriorityNormal+3, NULL);
 
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  
  //UI换页响应事件组使用低五位：0位：右；1位：左，2位：下，3位：上，4位：按钮
  UIResponseEvent = xEventGroupCreate();
  
  //Mqtt事件使用低3位，分别对应三个lightdemo topic：0位：switch，1位：multi，2位：cont
  MqttNotifyEvent = xEventGroupCreate();
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

