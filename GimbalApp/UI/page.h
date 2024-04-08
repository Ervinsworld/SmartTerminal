#ifndef __PAGE_H
#define __PAGE_H

#include "stm32f4xx_hal.h"

#define Xres 128  //oled x分辨率
#define Yres 64	  //oled y分辨率
#define MainBMPres 48   //主图标像素值
#define ArrowBMPres 16  //箭头图标像素值，只有滑动模式拥有箭头图标
#define PageNum 9
#define PosPageNum 5

/*
页面的类型总体来看有两种：
	主动类型：菜单中响应摇杆上下移动而循环刷新的页面
			 主动类型页面有两种：
				- pStatic 下有滑动框的主动类型
				- pSilde  下无滑动框（只有静态字符串）的主动类型
	被动类型：由主动类型页面的事件（因按下按钮）而触发的页面
			 被动类型页面只有一种：
				- nStatic

*/

typedef enum
{
    pStatic = 0,
	pSilde = 1,
	nStatic = 2,
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
	const uint8_t datanum; //页面可以携带的数据数量，滑动页面为页面的滑动条数，静态页面该值为0
	uint8_t nowadata;//页面传递的实时信息，只有滑动模式页面有信息，确认及静态模式页面该值默认为0
	const unsigned char* mainBMP;
}UIPage;

//页面操作函数
void PagesInfInit(void);
void setCurrentpage(PageID id);
UIPage getCurrentpage(void);
PageID getCurrentpageId(void);
int8_t setPagedata(uint8_t data);
void showPage(void);
void clearPage(void);
void clearMainIcon(void);
void clearBottom(void);
void showString(void);
void clearString(void);
void showArrows(void);
void clearArrows(void);
#endif
