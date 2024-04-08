#include "driver_joystick.h"
#include "FreeRTOS.h"
#include "queue.h"

void Joystick_Task(void *params){
	joystickValue value;
	extern QueueHandle_t JoyStickQueueHandle;
	while(1){
		value = JoyStickValueCal();
		xQueueSendToFront(JoyStickQueueHandle, &value, portMAX_DELAY);
	}
}
