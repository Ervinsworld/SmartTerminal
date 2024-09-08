#include "FreeRTOS.h"
#include "queue.h"

#ifndef _MOTOR__H
#define _MOTOR__H

#define _PI   3.14159265359
#define _PI_2 1.57079632679
#define _PI_4 0.78539816339
#define _3_PI_4 2.35619449019
#define _PI_8 0.39269908169
#define _3_PI_8 1.17809724509

#define motorID 1


float getMotorAngle(void);
float getMotorSpeed(void);
void MotorPid_Task(void *params);
void CalMotorData_Task(void *params);
void AnglePrint_Task(void *params);

#endif
