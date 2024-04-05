#ifndef __DRIVER_JOYSTICKER_H
#define __DRIVER_JOYSTICKER_H

#include "stm32f4xx_hal.h"

//摇杆X,Y轴值结构体，每轴数值范围：0~4096
struct JoystickValue{
	uint32_t xValue;
	uint32_t yValue;
};

void JoyStickInit(void);
struct JoystickValue JoyStickValueCal(void);
void Joystick_Test(void);
	
#endif
