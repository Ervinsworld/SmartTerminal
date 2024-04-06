#include "UI.h"
#include "Icons.c"
#include "stm32f4xx_hal.h"
#include "driver_oled.h"
#include <string.h>

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

const UIPage UIpages[10] = 
{	
{pStatic, 0, BMP_Eye_48x48},
{nStatic, 0, BMP_EyeBlocked_48x48},
{pSilde, 0, BMP_Meter_48x48},
{pStatic, 0, BMP_Flip_48x48},
{pSilde, 0, BMP_Camera_48x48},
{pStatic, 0, BMP_Power_48x48},
{nStatic, 0, BMP_Lock_48x48},
{nStatic, 0, BMP_Unlocked_48x48},
{nStatic, 0, BMP_Spinner_48x48},
};

//当前页面的结构体参数和当前页面的id
UIPage g_currentPage;
PageID g_currentId;

//主图标位置参数
static uint8_t g_mainIcon_Xstart;
static uint8_t g_mainIcon_XEnd;
static uint8_t g_mainIcon_Ystart;
static uint8_t g_mainIcon_YEnd;

//底部信息字符串信息参数
static uint8_t g_stringSize = 2;//字符大小恒定为2
static uint8_t g_string_Ypos = 6;//字符串显示y坐标恒定为6
static uint8_t g_string_Xpos;
static uint8_t g_string_LenRes;


/**********************************************************************
 * 函数名称： PageInfInit
 * 功能描述： 计算并初始化Page显示所需的全局变量
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
void PagesInfInit(){
	//计算主图标位置，保证主图标在画面中央
	g_mainIcon_Xstart = ((Xres-MainBMPres)>>1) - 1;
	g_mainIcon_XEnd = g_mainIcon_Xstart + MainBMPres;
	g_mainIcon_Ystart = 0;
	g_mainIcon_YEnd = (g_mainIcon_Ystart + MainBMPres)>>3;
	
	//底部信息字符串位置参数,皆为定值
	g_string_Ypos = 6;//字符串显示y坐标恒定为6
	g_stringSize = 2;//字符大小恒定为2
}

/**********************************************************************
 * 函数名称： setCurrentpage
 * 功能描述： 设置当前页面，写入全局变量
 * 输入参数： pageID
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
void setCurrentpage(PageID id){
	g_currentPage = UIpages[id];
	g_currentId = id;
}

/**********************************************************************
 * 函数名称： getCurrentpage
 * 功能描述： 获取当前页面结构体
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： UIPage
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
 UIPage getCurrentpage(){
	return g_currentPage;
}

/**********************************************************************
 * 函数名称： getCurrentpageId
 * 功能描述： 获取当前页面结构体Id
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： PageID
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
PageID getCurrentpageId(){
	return g_currentId;
}

/**********************************************************************
 * 函数名称： showPage
 * 功能描述： 页面显示
 * 输入参数： UIPage结构体，索引号
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
void showPage(){
	//若为滑动模式则显示箭头
	if(g_currentPage.InfMode == pSilde)
		showArrows();
	showString();
	OLED_DrawBMP(g_mainIcon_Xstart,g_mainIcon_Ystart,g_mainIcon_XEnd,g_mainIcon_YEnd,g_currentPage.mainBMP);
	//OLED_DrawBMP(39,0,87,6,BMP_Camera_48x48);
}

/**********************************************************************
 * 函数名称： clearPage
 * 功能描述： 清除整页数据，全屏清除太耗时
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
void clearPage(){
	clearMainIcon();
	clearBottom();
}

/**********************************************************************
 * 函数名称： clearMainIcon
 * 功能描述： 清除主图标48*48像素的区域
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
void clearMainIcon(){
	uint8_t i, j;
	for(i=0; i<6; i++){
		OLED_SetPos(g_mainIcon_Xstart,g_mainIcon_Ystart+i);
		for(j=0;j<MainBMPres;j++)
			WriteDat(0x00);
	}
}

/**********************************************************************
 * 函数名称： clearString
 * 功能描述： 清除底部字符串区域，默认size为2
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
void clearString(){
	uint8_t i, j;
	for(i=0; i<2; i++){
		OLED_SetPos(g_string_Xpos,g_string_Ypos+i);
		for(j=0;j<g_string_LenRes;j++)
			WriteDat(0x00);
	}
}

/**********************************************************************
 * 函数名称： showString
 * 功能描述： 计算字符信息传入全局变量，并显示底部字符串
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
void showString(){
	uint8_t len = strlen(bottomStr[g_currentId][g_currentPage.nowadata]);
	g_string_LenRes = len<<3;
	g_string_Xpos = ((Xres-g_string_LenRes)>>1) - 1;
	OLED_ShowStr(g_string_Xpos, g_string_Ypos, (unsigned char*)bottomStr[g_currentId][g_currentPage.nowadata], g_stringSize);
}

/**********************************************************************
 * 函数名称： clearBottom
 * 功能描述： 清除底部所有区域
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
void clearBottom(){
	clearArrows();
	clearString();
}
/**********************************************************************
 * 函数名称： clearArrows
 * 功能描述： 清除底部箭头区域
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
void clearArrows(){
	uint8_t i, j;
	for(i=0; i<2; i++){
		OLED_SetPos(0,6+i);
		for(j=0;j<ArrowBMPres;j++)
			WriteDat(0x00);
	}
	for(i=0; i<2; i++){
		OLED_SetPos(Xres-ArrowBMPres,6+i);
		for(j=0;j<ArrowBMPres;j++)
			WriteDat(0x00);
	}
}

/**********************************************************************
 * 函数名称： showArrows
 * 功能描述： 滑动模式的页面显示左右箭头
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
void showArrows(){
	OLED_DrawBMP(0,6,ArrowBMPres,8,BMP_Left_16x16);//显示左箭头
	OLED_DrawBMP(Xres-ArrowBMPres,6,Xres,8,BMP_Right_16x16);//显示右箭头
}
