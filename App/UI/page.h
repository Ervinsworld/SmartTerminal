#ifndef __PAGE_H
#define __PAGE_H

#include "stm32f4xx_hal.h"

/*
页面的类型有两种：
	父页面(主动类型,p开头)：菜单中响应摇杆上下移动而循环刷新的页面
			 主动类型页面有两种：
				- pStatic 最下栏有滑动框的页面
				- pSilde  最下栏无滑动框（只有静态字符串）的页面
				
	子页面(被动类型,n开头)：由主动类型页面的事件（因按下按钮）而触发的页面
			 被动类型页面有三种：
				- nStatic  只显示画面，画面本身和外设无交互,动画效果依靠多个nStatic页面的快速切换
				- nBar 	   有交互的，并且使用进度条交互
				- nDynamic 有交互的，效果需要单独实现，如游戏
*/


/************					页面相关枚举类				************/
typedef enum
{
    pStatic = 0,
	pSlide= 1,
	nStatic = 2,
	nBar = 3,	  //数值条页面
	nDynamic = 4, //动态页面，如游戏页面
}InterfaceMode;

//页面代号，共N种页面,在c文件中补充页面的具体信息，注意在c文件中补充时要一一对应
typedef enum
{
    Light1 = 1,
	Off,
	On,
	Light2,
	Bright1,
	Bright2,
	Bright3,
	Light3,
	Game,
	Settings,
	LightBar,  //Light3亮度条页面
	BriBar, //屏幕亮度条页面
	IntensBar //阻尼条页面
}PageID;

/************					页面信息结构体				************/
typedef struct UI_page{
	InterfaceMode InfMode;
	const uint8_t slideNum; //页面的滑动条的条数，只有pSlide模式页面有，其余为0
	uint8_t data;//pSlide页面传递换页信息，其余页面传递自定义信息
	const unsigned char* mainBMP;
}UIPage;


/************					页面操作函数					************/
//初始化方法
void PageInit(void);

//界面转移方法
int8_t PageDown(void);
int8_t PageUp(void);
int8_t SlideRight(void);
int8_t SlideLeft(void);
int8_t PageIn(void);
int8_t PageOut(void);
int8_t sonPageSwitch(PageID targetPage);//子页面的换页

//全局变量get/set方法
void setCurrentpage(PageID id);
UIPage getCurrentpage(void);
PageID getCurrentpageId(void);
int8_t setCurrentPagedata(uint8_t data);
int8_t setPagedata(uint8_t id, uint8_t data);

//数值条操作方法
void showbarFrame(void);//显示数值条外框
int8_t showbar(void);//根据pagedata显示数值条
//int8_t showbardata(void);//显示底部数据
int8_t showBottomData(int8_t data);
void clearBar(void);
int8_t barInit(void);

/*屏幕参数设置*/
void setScreenBri(uint8_t value);

#endif
