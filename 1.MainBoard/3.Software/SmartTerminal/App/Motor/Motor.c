#include "driver_motorComm.h"
#include "pid.h"
#include "Motor.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core

extern QueueHandle_t TargetAngleQueueHandle;
extern QueueHandle_t AngleDiffQueueHandle;
extern SemaphoreHandle_t MotorPidSemaphore;
//extern QueueHandle_t MotorRawQueueHandle;

/**********************************************************************
 * 函数名称： MotorPid_Task
 * 功能描述： FreeRTOS电机任务，实时运行pid算法，1ms周期
 * 输入参数： params
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/17	     V1.0	  Ervin	      创建
 ***********************************************************************/
void MotorPid_Task(void *params){
	float targetAngle = 0;
	MotorPIDInit();
	xSemaphoreGive(MotorPidSemaphore);//初始状态置信号量为1
	while(1){
		xSemaphoreTake(MotorPidSemaphore, portMAX_DELAY);
		xQueuePeek(TargetAngleQueueHandle, &targetAngle, 0);
		SendMessage2Motor(cascade_loop(targetAngle, getMotorAngle(), getMotorSpeed()), motorID);
		//printf("%f, %f, %f\n", angle, baseAngle, targetAngle);
		xSemaphoreGive(MotorPidSemaphore);
		vTaskDelay(pdMS_TO_TICKS(1));
	}
}

/**********************************************************************
 * 函数名称： AnglePrint_Task
 * 功能描述： 打印电机的各项角度信息，用于调试
 * 输入参数： params
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/17	     V1.0	  Ervin	      创建
 ***********************************************************************/
void AnglePrint_Task(void *params){
	float targetAngle = 0;
	float diffAngle = 0;
	while(1){
		//xQueuePeek(MotorRawQueueHandle, &rxData, 0);
		xQueuePeek(TargetAngleQueueHandle, &targetAngle, 0);
		xQueuePeek(AngleDiffQueueHandle, &diffAngle, 0);
		printf("%f, %f, %f\n", getMotorAngle(), diffAngle, targetAngle);//物理角度，角度差值，目标角度
		//printf("%f, %f\n", getMotorAngle(), getMotorSpeed());
		vTaskDelay(50);
	}
}
