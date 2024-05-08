#ifndef __UI_H
#define __UI_H

#include "stm32f4xx_hal.h"
#include "page.h"

#define EMPTY -1

//将nStatic页面的id和对应的页面角度矩阵对应起来
typedef struct anglemap{
	PageID pageId;
	float **switchMat;
}angleMap;

/*FreeRTOS任务函数*/
void UI_Task(void *params);
void UIAction_Task(void *params);
void UIPrint_Task(void *params);

/*操作函数*/
void UI_Init(void);
#endif
