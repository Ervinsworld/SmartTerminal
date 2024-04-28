#include "driver_motorComm.h"
#include "pid.h"
#include "Motor.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups

extern QueueHandle_t TargetAngleQueueHandle;
extern QueueHandle_t AngleDiffQueueHandle;
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
	while(1){
		xQueuePeek(TargetAngleQueueHandle, &targetAngle, 0);
		SendMessage2Motor(cascade_loop(targetAngle, g_currentMotorInf.angle, g_currentMotorInf.speed), motorID);
		//printf("%f, %f, %f\n", angle, baseAngle, targetAngle);
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
		printf("%f, %f, %f\n", g_currentMotorInf.angle, diffAngle, targetAngle);//物理角度，角度差值，目标角度
		vTaskDelay(50);
	}
}
