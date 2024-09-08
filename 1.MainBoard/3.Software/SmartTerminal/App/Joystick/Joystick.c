#include "driver_joystick.h"
#include "page.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups

extern EventGroupHandle_t UIResponseEvent;

/**********************************************************************
 * 函数名称： Joystick_Task
 * 功能描述： FreeRTOS摇杆任务,每100tick去DMA队列中计算一次摇杆值
 * 输入参数： params
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/17	     V1.0	  Ervin	      创建
 ***********************************************************************/
void Joystick_Task(void *params){
	joystickValue value;
	while(1){
		value = JoyStickValueCal();
		if(value.xValue<500)
			xEventGroupSetBits(UIResponseEvent, 1<<0);
		else if(value.xValue>3500)
			xEventGroupSetBits(UIResponseEvent, 1<<1);
		else if(value.yValue>3500)
			xEventGroupSetBits(UIResponseEvent, 1<<2);
		else if(value.yValue<500)
			xEventGroupSetBits(UIResponseEvent, 1<<3);
		vTaskDelay(100);
	}
}

void ButtonTest_Task(void *params){
	while(1){
		//xEventGroupWaitBits(UIResponseEvent, 1<<0|1<<1|1<<2|1<<3|1<<4, pdTRUE, pdFALSE, portMAX_DELAY);
		//xEventGroupWaitBits(UIResponseEvent, 1<<4, pdTRUE, pdFALSE, portMAX_DELAY);
		//printf("Button is pressed!\n");
		vTaskDelay(3000);
		xEventGroupSetBits(UIResponseEvent, 1<<3);
	}
}
