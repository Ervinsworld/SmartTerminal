#include "driver_motorComm.h"
#include "pid.h"
#include "Motor.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups


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
	float targetAngle = 0;
	MotorPIDInit();
	while(1){
		xQueuePeek(TargetAngleQueueHandle, &targetAngle, 0);
		SendMessage2Motor(cascade_loop(targetAngle, g_currentMotorInf.angle, g_currentMotorInf.speed), motorID);
		//printf("%f, %f, %f\n", angle, baseAngle, targetAngle);
		vTaskDelay(pdMS_TO_TICKS(1));
	}
}
