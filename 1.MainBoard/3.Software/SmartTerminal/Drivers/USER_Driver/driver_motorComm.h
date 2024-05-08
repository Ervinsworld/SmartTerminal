#ifndef __DRIVER_MOTORCOMM_H
#define __DRIVER_MOTORCOMM_H

#include "stm32f4xx_hal.h"

typedef struct MotorStatus{
	float angle;  
	float speed;        
}MotorInf;


float getMotorAngle(void);
float getMotorSpeed(void);
void MotorCommInit(void);
void SendMessage2Motor(float voltage, uint8_t motorID);
void MotorComm_Test(void);	

#endif

