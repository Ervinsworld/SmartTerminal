#include "driver_oled.h"
#include "UI.h"
#include "menu.h"
/**********************************************************************
 * 函数名称： MenuInit
 * 功能描述： 初始化UI菜单，并显示首页
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
void MenuInit(){
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
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
void PageDown(){
	extern UIPage UIpages[10];
	uint8_t id = getCurrentpageId();
	UIPage page = getCurrentpage();
	do{
		id++;
		if(id==PageNum-1){
			id = 0;
		}
	}while(UIpages[id].InfMode==nStatic);
	setCurrentpage(id);
	clearPage();
	showPage();
}

/**********************************************************************
 * 函数名称： PageUp
 * 功能描述： 翻上一页
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
void PageUp(){
	extern UIPage UIpages[10];
	uint8_t id = getCurrentpageId();
	UIPage page = getCurrentpage();
	do{
		id--;
		if(id==0){
			id = PageNum-1;
		}
	}while(UIpages[id].InfMode==nStatic);
	setCurrentpage(id);
	clearPage();
	showPage();
}
