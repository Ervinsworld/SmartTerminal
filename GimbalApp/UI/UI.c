#include "UI.h"
#include "Icons.c"
#include "stm32f4xx_hal.h"
#include "driver_oled.h"
#include <string.h>

PageID id;
InterfaceMode Mode;

//页面底部的字符串信息，根据UI_page结构体数组的数组索引值索引字符串信息
const char* bottomStr[10][3] = 
{
	{"Switch", "", ""},
	{"Switch", "", ""},
	{"Low", "Meduim", "High"},
	{"Flip", "", ""},
	{"OFF", "ON", ""},
	{"Power off", "", ""},
	{"Hold to Unlock", "", ""},
	{"Unlocking...", "", ""},
	{"Initing...", "", ""},
};

UIPage UIpages[10] = 
{	
{EnterMode, 0, BMP_Eye_48x48},
{EnterMode, 0, BMP_EyeBlocked_48x48},
{SlideMode, 0, BMP_Meter_48x48},
{EnterMode, 0, BMP_Flip_48x48},
{SlideMode, 0, BMP_Camera_48x48},
{EnterMode, 0, BMP_Power_48x48},
{EnterMode, 0, BMP_Lock_48x48},
{StaticMode, 0, BMP_Unlocked_48x48},
{StaticMode, 0, BMP_Spinner_48x48},
};



//根据字符串长度计算字符的X坐标以保证字符在画面中央
uint8_t String_XPos(const char * str){
	uint8_t len = strlen(str);
	return ((Xres-(len<<3))>>1) - 1;
}


/**********************************************************************
 * 函数名称： showPage
 * 功能描述： 页面显示函数
 * 输入参数： UIPage结构体，索引号
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
void showPage(UIPage* page, PageID id){
	UIPage tmpPage = page[id];
	//计算主图标位置，保证主图标在画面中央
	uint8_t mainIcon_Xstart = ((Xres-MainBMPres)>>1) - 1;
	uint8_t mainIcon_XEnd = mainIcon_Xstart + MainBMPres;
	uint8_t mainIcon_Ystart = 0;
	uint8_t mainIcon_YEnd = (mainIcon_Ystart + MainBMPres)>>3;
	
	//底部信息字符串位置参数
	uint8_t String_Ypos = 6;//字符串显示y坐标恒定为6
	uint8_t StringSize = 2;//字符大小恒定为2
	uint8_t String_Xpos = String_XPos(bottomStr[id][tmpPage.nowadata]);

	//OLED_ShowStr(40, 6, pstr, 2);
	OLED_ShowStr(String_Xpos, String_Ypos, (unsigned char*)bottomStr[id][tmpPage.nowadata], StringSize);
	OLED_DrawBMP(mainIcon_Xstart,mainIcon_Ystart,mainIcon_XEnd,mainIcon_YEnd,tmpPage.mainBMP);
}

