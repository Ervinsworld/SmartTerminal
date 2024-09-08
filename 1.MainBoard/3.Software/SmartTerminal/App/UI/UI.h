#ifndef __UI_H
#define __UI_H

#include "stm32f4xx_hal.h"
#include "page.h"

/*FreeRTOS������*/
void UI_Task(void *params);
void UIAction_Task(void *params);
void UIPrint_Task(void *params);

/*��������*/
void UI_Init(void);
#endif
