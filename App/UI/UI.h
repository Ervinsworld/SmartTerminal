#ifndef __UI_H
#define __UI_H

#include "stm32f4xx_hal.h"

/*FreeRTOS任务函数*/
void UI_Task(void *params);
void UIAction_Task(void *params);
void UIPrint_Task(void *params);

/*操作函数*/
void UI_Init(void);
#endif
