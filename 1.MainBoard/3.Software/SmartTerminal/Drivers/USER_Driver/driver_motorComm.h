#ifndef __DRIVER_MOTORCOMM_H
#define __DRIVER_MOTORCOMM_H

#include "stm32f4xx_hal.h"

void MotorCommInit(void);
void SendMessage2Motor(float voltage, uint8_t motorId);
void MotorComm_Test(void);	

#endif

