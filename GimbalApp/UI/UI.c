#include "driver_oled.h"
#include "driver_joystick.h"
#include "page.h"
#include "UI.h"
#include "FreeRTOS.h"

/**********************************************************************
 * 函数名称： UI_Task
 * 功能描述： FreeRTOS的任务，用于页面的切换/响应处理操作
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/

void UI_Task(void *params){
	joystickValue value;
	int8_t SlideRet;//滑动操作的返回值
	int8_t PageRet;//翻页操作的返回值
	UI_Init();//初始化UI
	while(1){
		value = JoyStickValueCal();
		SlideRet = -1;//默认为未成功状态
		PageRet = -1;//默认为未成功状态
		//优先处理滚动条
		if(value.xValue>3500){
			SlideRet = SlideRight();
		}
		else if(value.xValue<500){
			SlideRet = SlideRight();
		}
		//若成功滑动了
		if(SlideRet == 0){
			vTaskDelay(500);
		}
		//处理翻页操作//
		if(value.yValue>3500){
			PageRet = PageDown();
		}
		else if(value.yValue<500){
			PageRet = PageUp();
		}
		//若成功滑动了
		if(PageRet == 0){
			vTaskDelay(500);
		}
		vTaskDelay(20);
	}
}




/**********************************************************************
 * 函数名称： UI_Init
 * 功能描述： 初始化UI菜单，并显示首页
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
void UI_Init(){
	OLED_Init();
	OLED_CLS();
	PagesInfInit();
	setCurrentpage(SwitchPage_EYEON);//将switchpage作为开机初始页面
	showPage();
}

/**********************************************************************
 * 函数名称： PageDown
 * 功能描述： 翻下一页
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0表示成功，-1表示失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/7	     V1.0	  Ervin	      创建
 ***********************************************************************/
int8_t PageDown(){
	extern UIPage UIpages[10];
	uint8_t id = getCurrentpageId();
	UIPage page = getCurrentpage();
	//若该页是被动状态，则无法执行翻页操作
	if(page.InfMode==nStatic)
		return -1;
	do{
		if(id==PageNum-1){
			id = 0;
		}
		else{
			id++;
		}
	}while(UIpages[id].InfMode==nStatic);
	setCurrentpage(id);
	clearPage();
	showPage();
	return 0;
}

/**********************************************************************
 * 函数名称： PageUp
 * 功能描述： 翻上一页
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0表示成功，-1表示失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/7      V1.0	  Ervin	      创建
 ***********************************************************************/
int8_t PageUp(){
	extern UIPage UIpages[10];
	uint8_t id = getCurrentpageId();
	UIPage page = getCurrentpage();
	//若该页是被动状态，则无法执行翻页操作
	if(page.InfMode==nStatic)
		return -1;
	do{
		if(id==0){
			id = PageNum-1;
		}
		else{
			id--;
		}

	}while(UIpages[id].InfMode==nStatic);
	setCurrentpage(id);
	clearPage();
	showPage();
	return 0;
}

/**********************************************************************
 * 函数名称： SlideRight
 * 功能描述： 向右滑动
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0表示成功，-1表示失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/7	     V1.0	  Ervin	      创建
 ***********************************************************************/
int8_t SlideRight(){
	//uint8_t id = getCurrentpageId();
	UIPage page = getCurrentpage();
	//若该页不是滑动页面，则无法执行滑动
	if(page.InfMode!=pSilde)
		return -1;
	uint8_t num = page.datanum;
	uint8_t data = page.nowadata;
	if(data == num-1){
		data = 0;
	}
	else{
		data++;
	}
	setPagedata(data);
	clearString();
	showString();
	return 0;
}

/**********************************************************************
 * 函数名称： SlideLeft
 * 功能描述： 向右滑动
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0表示成功，-1表示失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/7	     V1.0	  Ervin	      创建
 ***********************************************************************/
int8_t SlideLeft(){
	//uint8_t id = getCurrentpageId();
	UIPage page = getCurrentpage();
	//若该页不是滑动页面，则无法执行滑动
	if(page.InfMode!=pSilde)
		return -1;
	uint8_t num = page.datanum;
	uint8_t data = page.nowadata;
	if(data == 0){
		data = num-1;
	}
	else{
		data--;
	}
	setPagedata(data);
	clearString();
	showString();
	return 0;
}

