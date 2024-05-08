#ifndef __DRIVER_JOYSTICKER_H
#define __DRIVER_JOYSTICKER_H

#include "stm32f4xx_hal.h"

//摇杆X,Y轴值结构体，每轴数值范围：0~4096,中值2000-2100
//X轴左小右大，Y轴上小下大
typedef struct JoystickValue{
	uint32_t xValue;
	uint32_t yValue;
}joystickValue;

typedef enum {
	free = 0,
	pressed = 1,
}buttonState;

void JoyStickInit(void);
joystickValue JoyStickValueCal(void);
void Joystick_Test(void *params);
	
#endif
