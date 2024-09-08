#include "driver_joystick.h"
#include "page.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups

extern EventGroupHandle_t UIResponseEvent;

/**********************************************************************
 * �������ƣ� Joystick_Task
 * ���������� FreeRTOSҡ������,ÿ100tickȥDMA�����м���һ��ҡ��ֵ
 * ��������� params
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/17	     V1.0	  Ervin	      ����
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
