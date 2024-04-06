#ifndef __UI_H
#define __UI_H

#include "stm32f4xx_hal.h"

#define Xres 128  //oled x分辨率
#define Yres 64	  //oled y分辨率
#define MainBMPres 48   //主图标像素值
#define ArrowBMPres 16  //箭头图标像素值，只有滑动模式拥有箭头图标


//页面的模式，共三种，滑动模式，确认模式以及静态模式
typedef enum
{
    SlideMode = 0,
	EnterMode = 1,
	StaticMode = 2,
}InterfaceMode;

//页面代号，共九种页面
typedef enum
{
    SwitchPage_EYEON = 0,
	SwitchPage_EYEOFF,
	SensPage,
	FlipPage,
	CameraPage,
	PoweroffPage,
	LockPage,
	UnlockPage,
	InitPage,
}PageID;

//页面信息结构体
typedef struct UI_page{
	InterfaceMode InfMode;
	uint8_t nowadata;//页面传递的实时信息，只有滑动模式页面有信息，确认及静态模式页面该值默认为0
	const unsigned char* mainBMP;
}UIPage;

//页面操作函数
void showPage(UIPage* page, PageID id);

#endif
