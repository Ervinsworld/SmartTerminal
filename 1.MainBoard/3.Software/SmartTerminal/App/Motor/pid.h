#ifndef PID_H
#define PID_H

#include "stm32f4xx_hal.h"

typedef struct
{
   	float kp,ki,kd;//三个系数
    float error,lastError;//误差、上次误差
    float integral,maxIntegral;//积分、积分限幅
    float output,maxOutput;//输出、输出限幅	
}PID;

/******************************************************************************/
float speed_loop(float target_speed, float current_speed);
float cascade_loop(float target_angle, float current_angle, float current_speed);
void MotorPIDInit(void);
void setIntens(uint8_t value);
/******************************************************************************/

#endif

