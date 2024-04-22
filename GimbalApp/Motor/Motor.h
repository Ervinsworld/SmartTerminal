#ifndef _MOTOR__H
#define _MOTOR__H

#define _PI   3.14159265359
#define _PI_2 1.57079632679
#define _PI_4 0.78539816339
#define _PI_8 0.39269908169
#define _3_PI_8 1.1780972450961

#define motorID 1

void MotorPid_Task(void *params);
void Motor_Task(void *params);

#endif
