#include "driver_motorComm.h"
#include "pid.h"
#include "Motor.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups

extern MotorInf g_currentMotorInf;

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
	extern QueueHandle_t TargetAngleQueueHandle;
	float targetAngle;
	MotorPIDInit();
	while(1){
		xQueueReceive(TargetAngleQueueHandle, &targetAngle, 0);
		SendMessage2Motor(cascade_loop(targetAngle, g_currentMotorInf.angle, g_currentMotorInf.speed), motorID);
		//printf("%f, %f\n", g_currentMotorInf.angle, g_currentMotorInf.speed);
		vTaskDelay(pdMS_TO_TICKS(1));
	}
}


void Motor_Task(void *params){
	extern EventGroupHandle_t UIResponseEvent;
	vTaskDelay(2000);//电机上电初始化时任务不运行
	while(1){
		if(g_currentMotorInf.angle<-_PI_4){
			xEventGroupSetBits(UIResponseEvent, 1<<4);
		}
		vTaskDelay(100);
	}
}