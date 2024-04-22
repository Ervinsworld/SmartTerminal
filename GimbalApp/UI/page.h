#ifndef __PAGE_H
#define __PAGE_H

#include "stm32f4xx_hal.h"


#define Xres 128  //oled x分辨率
#define Yres 64	  //oled y分辨率
#define MainBMPres 48   //主图标像素值
#define ArrowBMPres 16  //箭头图标像素值，只有滑动模式拥有箭头图标

#define PageNum 10
#define FatherPageNum 5
#define MaxSonPageNum 4

#define BarXStart 14
#define BarYStart 2
#define BarLength 100
#define BarWidth  1
/*
页面的类型有两种：
	主动类型：菜单中响应摇杆上下移动而循环刷新的页面
			 主动类型页面有两种：
				- pStatic 最下栏有滑动框的主动类型
				- pSilde  最下栏无滑动框（只有静态字符串）的主动类型
	被动类型：由主动类型页面的事件（因按下按钮/旋转电机等）而触发的页面
			 被动类型页面也有两种：
				- nStatic  只显示无交互
				- nDynamic 有交互的
*/


/************					页面相关枚举类				************/
typedef enum
{
    pStatic = 0,
	pSlide= 1,
	nStatic = 2,
	nDynamic = 3
}InterfaceMode;

//页面代号，共十种页面,在c文件中补充页面的具体信息
typedef enum
{
    Light = 1,
	Off,
	On,
	Mode,
	Bright1,
	Bright2,
	Bright3,
	Window,
	MotorPlay,
	Game,
}PageID;

/************					页面信息结构体				************/
typedef struct UI_page{
	InterfaceMode InfMode;
	const uint8_t slideNum; //页面的滑动条的条数，只有pSlide模式页面有，其余为0
	uint8_t data;//pSlide页面传递换页信息，其余页面传递自定义信息
	const unsigned char* mainBMP;
}UIPage;


/************					页面操作函数					************/
//信息初始化方法
void PagesInfInit(void);

//界面转移方法
int8_t PageDown(void);
int8_t PageUp(void);
int8_t SlideRight(void);
int8_t SlideLeft(void);
int8_t PageIn(void);
int8_t PageOut(void);

//全局变量get/set方法
void setCurrentpage(PageID id);
UIPage getCurrentpage(void);
PageID getCurrentpageId(void);
int8_t setCurrentPagedata(uint8_t data);
int8_t setPagedata(uint8_t id, uint8_t data);

//位图显示，清除方法
void showPage(void);
void clearPage(void);

//数值条操作方法
void showbarFrame(void);
void showbar(void);
int8_t showbardata(void);

//不需要使用的子方法
void clearMainIcon(void);
void clearBottom(void);
void showString(void);
void clearString(void);
void showArrows(void);
void clearArrows(void);
void showBackArrow(void);
void clearBackArrow(void);

char* Int2String(int num,char *str);
#endif
