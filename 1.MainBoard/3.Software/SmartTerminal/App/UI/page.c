#include "page.h"
#include "stm32f4xx_hal.h"
#include "driver_oled.h"
#include <string.h>
#include "FreeRTOS.h"
#include "Icons.c"

#define Xres 128  //oled x分辨率
#define Yres 64	  //oled y分辨率
#define MainBMPres 48   //主图标像素值
#define ArrowBMPres 16  //箭头图标像素值，只有滑动模式拥有箭头图标

#define PageNum 13
#define FatherPageNum 5 //父页面的数量
#define MaxSonPageNum 4	//最多的子页面的数量

#define BarXStart 14
#define BarYStart 3
#define BarLength 100
//#define BarWidth  1

//由于oled操作很耗时，在oled操作密集时，需要使用短暂延时间隔各，以防止丢帧;该宏给vTaskDelay调用，单位为tick
#define OLED_DELAY 50	

/************					页面相关表				************/
//父子页面关系表，二维数组每行的第一个元素是父页面，其余是子页面，索引号和UI_page结构体的data对应
const uint8_t PageMapTable[FatherPageNum][MaxSonPageNum] = 
{
	{Light1, Off, On},
	{Light2, Bright1, Bright2, Bright3},
	{Light3, LightBar},
	{Settings, BriBar, IntensBar},
};

//页面底部的字符串信息，索引值为.h文件中页面的ID
//！！！注意：对于pSlide模式的页面，其索引号一定要和和UI_page结构体的data对应
const char* bottomStr[PageNum][3] = 
{
	{"Switch", "", ""},
	{"OFF", "", ""},
	{"ON", "", ""},
	{"Mode", "", ""},
	{"Low", "", ""},
	{"Medium", "", ""},
	{"High", "", ""},
	{"Bar", "", ""},
	{"Block", "Car"},
	{"Brightness", "Intensity"},
};

//索引值为.h文件中页面的ID
UIPage UIpages[PageNum] = 
{	
{pStatic, 0, 1, BMP_LIGHT1_48x48},//data值1表示灯光关闭，2表示灯光开启
{nStatic, 0, 0, BMP_OFF_48x48},
{nStatic, 0, 0, BMP_ON_48x48},
{pStatic, 0, 1, BMP_LIGHT2_48x48},//data值1，2，3代表亮度1，2，3
{nStatic, 0, 0, BMP_BRIGHT1_48x48},
{nStatic, 0, 0, BMP_BRIGHT2_48x48},
{nStatic, 0, 0, BMP_BRIGHT3_48x48},
{pStatic, 0, 1, BMP_LIGHT3_48x48},//data恒为1
{pSlide, 2, 1, BMP_SWITCH_48x48},
{pSlide, 2, 1, BMP_SETTINGS_48x48},
{nBar, 0, 0, NULL},//lightBar页面，data存储bar的值，0-100，精度1, 开机默认为0
{nBar, 0, 100,NULL}, //BriBar页面，data存储bar的值，0-100，精度1, 开机默认为100
{nBar, 0, 50,NULL}, //DampingBar页面，data存储bar的值，0-100，精度1, 开机默认为50
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

//函数声明
static void PagesInfInit(void);
static int8_t getSonPage(PageID faPage);
static int8_t getFaPage(PageID sonPage);
static UIPage getPage(PageID id);
static void showPage(void);
static void clearPage(void);
static void clearMainIcon(void);
static void clearBottom(void);
static void showString(void);
static void clearString(void);
static void showArrows(void);
static void clearArrows(void);
static void showBackArrow(void);
static void clearBackArrow(void);
char* Int2String(int num,char *str);

/**********************************************************************
 * 函数名称： PageInit
 * 功能描述： Page页面开机初始化
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
void PageInit(){
	OLED_Init();
	OLED_CLS();
	PagesInfInit();
	setCurrentpage(Light1);//将switchpage作为开机初始页面
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
	uint8_t id = g_currentId;
	UIPage page = g_currentPage;
	//若该页是被动状态，则无法执行翻页操作
	if(page.InfMode==nStatic || page.InfMode==nBar || page.InfMode==nDynamic)
		return -1;
	do{
		if(id==PageNum){
			id = 1;
		}
		else{
			id++;
		}
	}while(getPage(id).InfMode==nStatic || getPage(id).InfMode==nBar || getPage(id).InfMode==nDynamic);
	//clearPage();
	clearString();
	if(page.InfMode == pSlide)
		clearArrows();
	setCurrentpage(id);
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
	uint8_t id = g_currentId;
	UIPage page = g_currentPage;
	//若该页是被动状态，则无法执行翻页操作
	if(page.InfMode==nStatic || page.InfMode==nBar || page.InfMode==nDynamic)
		return -1;
	do{
		if(id==1){
			id = PageNum;
		}
		else{
			id--;
		}

	}while(getPage(id).InfMode==nStatic || getPage(id).InfMode==nBar || getPage(id).InfMode==nDynamic);
	//clearPage();
	clearString();
	if(page.InfMode == pSlide)
		clearArrows();
	setCurrentpage(id);
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
	if(page.InfMode!=pSlide)
		return -1;
	uint8_t num = page.slideNum;
	uint8_t data = page.data;
	if(data == num){
		data = 1;
	}
	else{
		data++;
	}
	setCurrentPagedata(data);
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
	if(page.InfMode!=pSlide)
		return -1;
	uint8_t num = page.slideNum;
	uint8_t data = page.data;
	if(data == 1){
		data = num;
	}
	else{
		data--;
	}
	setCurrentPagedata(data);
	clearString();
	showString();
	return 0;
}

/**********************************************************************
 * 函数名称： PageIn
 * 功能描述： 进入子页面
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： -1为失败,否则返回0
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
int8_t PageIn(){
	int8_t sonPage;
	sonPage = getSonPage(g_currentId);
	if(sonPage == -1)
		return -1;
	clearString();
	if(g_currentPage.InfMode == pSlide)
		clearArrows();
	setCurrentpage(sonPage);
	if(g_currentPage.InfMode == nBar)
		clearMainIcon();
	showPage();
	return 0;
}



/**********************************************************************
 * 函数名称： PageOut
 * 功能描述： 返回父页面
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： -1为失败，0为成功
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
int8_t PageOut(){
	int8_t faPage;
	faPage = getFaPage(g_currentId);
	if(faPage == -1)
		return -1;
	clearString();
	clearBackArrow();
	if(g_currentPage.InfMode == nBar)
		clearBar();
	setCurrentpage(faPage);
	showPage();
	return 0;
}


/**********************************************************************
 * 函数名称： sonPageSwitch
 * 功能描述： 用于子页面的切换
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： -1为失败，0为成功
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
int8_t sonPageSwitch(PageID targetPage){
	int i, j;
	int currentPageFaId = -1, targetPageFaId = -1;//记录现页面和父页面的id
	uint8_t targetPageNum;
	//若现页面不是nStatic，返回-1
	if(g_currentPage.InfMode!=nStatic)
		return -1;
	for(i=0; i<FatherPageNum; i++){
		for(int j=1; j<MaxSonPageNum; j++){
			if(PageMapTable[i][j] == g_currentId){
				currentPageFaId = i;
			}
			else if(PageMapTable[i][j] == targetPage){
				targetPageFaId = i;
				targetPageNum = j;
			}
		}
	}
	//判断页面是否有共同父页面，若否则返回-1
	if(currentPageFaId!=targetPageFaId)
		return -1;
	//修改父页面的data值
	setPagedata(PageMapTable[targetPageFaId][0], targetPageNum);
	//执行换页操作
	clearString();
	//clearMainIcon();
	setCurrentpage(targetPage);
	showString();
	OLED_DrawBMP(g_mainIcon_Xstart,g_mainIcon_Ystart,g_mainIcon_XEnd,g_mainIcon_YEnd,g_currentPage.mainBMP);
	return 0;
}

/**********************************************************************
 * 函数名称： setScreenBri
 * 功能描述： 设置页面亮度
 * 输入参数： 0x00~0xff
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/
void setScreenBri(uint8_t value){
	//g_currentPage = UIpages[id];
	WriteCmd(0x81); //--set contrast control register
    WriteCmd(value); //亮度调节 0x00~0xff
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
	//g_currentPage = UIpages[id];
	memcpy(&g_currentPage, &UIpages[id-1], sizeof(g_currentPage));
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
 * 函数名称： setCurrentPagedata
 * 功能描述： 设置当前页面的数据值
 * 输入参数： data
 * 输出参数： 无
 * 返 回 值： -1表示失败，0表示成功
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/7	     V1.0	  Ervin	      创建
 ***********************************************************************/
int8_t setCurrentPagedata(uint8_t data){
	//判断设定值是否超过了数量值，若超过不允许操作，返回-1
	if(g_currentPage.InfMode == pSlide && data > g_currentPage.slideNum)
		return -1;
	else{
		g_currentPage.data = data;
		UIpages[g_currentId-1].data = data;
	}
	return 0;
}

/**********************************************************************
 * 函数名称： setPagedata
 * 功能描述： 设置指定页面的数据值
 * 输入参数： id, data
 * 输出参数： 无
 * 返 回 值： -1表示失败，0表示成功
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/7	     V1.0	  Ervin	      创建
 ***********************************************************************/
int8_t setPagedata(uint8_t id, uint8_t data){
	//判断设定值是否超过了数量值，若超过不允许操作，返回-1
	UIPage page = UIpages[id-1];
	if(page.InfMode == pSlide && data >= page.slideNum)
		return -1;
	else 
		UIpages[id-1].data = data;
	return 0;
}

/**********************************************************************
 * 函数名称： getPagedata
 * 功能描述： 获取指定页面的数据值
 * 输入参数： id
 * 输出参数： 无
 * 返 回 值： data， -1表示错误
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/9/4	     V1.0	  Ervin	      创建
 ***********************************************************************/
int8_t getPagedata(uint8_t id){
	//判断设定值是否超过了数量值，若超过不允许操作，返回-1
	UIPage page = UIpages[id-1];
	if(page.InfMode == pSlide)
		return -1;
	return page.data;
};


/**********************************************************************
 * 函数名称： showBottomData
 * 功能描述： 数值条底部数值的显示函数
 * 输入参数： int
 * 输出参数： 无
 * 返 回 值： 0为成功，-1为失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/19	     V1.0	  Ervin	      创建
 ***********************************************************************/
int8_t showBottomData(int8_t data){
	char s[7] = {0};
	char result_s[7] = " ";
	if(g_currentPage.InfMode!=nBar)
		return -1;
	Int2String(data, s);
	//给转换完成的数字前后都加上一个空格“ ”
	strcat(s, " ");
	strcat(result_s, s);
	//计算字符串显示参数并显示字符
	uint8_t len = strlen(result_s);
	g_string_LenRes = len<<3;
	g_string_Xpos = ((Xres-g_string_LenRes)>>1) - 1;
	OLED_ShowStr(g_string_Xpos, g_string_Ypos, (unsigned char*)result_s, g_stringSize);
	return 0;
}


/**********************************************************************
 * 函数名称： showbar
 * 功能描述： 数值条显示函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/19	     V1.0	  Ervin	      创建
 ***********************************************************************/

int8_t showbar(){
//	if(g_currentId != LightBar)
//		return -1;
	if(g_currentPage.InfMode != nBar)
		return -1;
	static uint8_t last_value = 0;
	int8_t error;
	uint8_t i;
	error = g_currentPage.data - last_value;
	if(error>0){
		OLED_SetPos(BarXStart+last_value,BarYStart);
		for(i=0;i<=error;i++)
			WriteDat(0xFF);
	}
	else if(error<0){
		OLED_SetPos(BarXStart+g_currentPage.data,BarYStart);
		//判断上次的值，设置for循环的开始和结束值，以禁止删除bar的左右边框
		for(i=0;i<=-error+1;i++){
			WriteDat(0xC3);
		}
	}
	OLED_SetPos(BarXStart+101,BarYStart);
	WriteDat(0xFF);
	WriteDat(0xFF);
	last_value = g_currentPage.data;
	return 0;
}


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
static void PagesInfInit(){
	//计算主图标位置，保证主图标在画面中央
	g_mainIcon_Xstart = ((Xres-MainBMPres)>>1) - 1;
	g_mainIcon_XEnd = g_mainIcon_Xstart + MainBMPres;
	g_mainIcon_Ystart = 0;
	g_mainIcon_YEnd = (g_mainIcon_Ystart + MainBMPres)>>3;
	
	//底部信息字符串位置参数,皆为定值
	g_string_Ypos = 6;//字符串显示y坐标恒定为6
	g_stringSize = 2;//字符大小恒定为2
}

static UIPage getPage(PageID id){
	return UIpages[id-1];
}

static int8_t getSonPage(PageID faPage){
	int i;
	for(i=0; i<FatherPageNum; i++){
	//查询页面映射关系表
		if(PageMapTable[i][0] == faPage)
			return PageMapTable[i][g_currentPage.data];
	}
	return -1;
}

static int8_t getFaPage(PageID sonPage){
	int i, j;
	for(i=0; i<FatherPageNum; i++){
		for(int j=1; j<MaxSonPageNum; j++){
			if(PageMapTable[i][j] == sonPage)
				return PageMapTable[i][0];
		}
	}
	return -1;
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
static void showPage(){
	if(g_currentPage.InfMode == nBar){
		showBackArrow();
		vTaskDelay(OLED_DELAY);
		showbarFrame();
		barInit();
		vTaskDelay(OLED_DELAY);
		//showBottomData(getCurrentpage().data);	
		return;
	}
	//若为滑动模式则显示箭头
	else if(g_currentPage.InfMode == pSlide)
		showArrows();
	//若为静态模式则显示返回箭头
	else if(g_currentPage.InfMode == nStatic)
		showBackArrow();
	OLED_DrawBMP(g_mainIcon_Xstart,g_mainIcon_Ystart,g_mainIcon_XEnd,g_mainIcon_YEnd,g_currentPage.mainBMP);
	showString();
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
static void clearPage(){
	if(g_currentPage.InfMode == nBar){
		vTaskDelay(OLED_DELAY);
		clearBackArrow();
		vTaskDelay(OLED_DELAY);
		clearBar();
		vTaskDelay(OLED_DELAY);
		clearString();
		vTaskDelay(OLED_DELAY);
		return;
	}
	else if(g_currentPage.InfMode == pSlide)
		clearArrows();
	else if(g_currentPage.InfMode == nStatic)
		clearBackArrow();
	clearMainIcon();
	clearString();
}

/**********************************************************************
 * 函数名称： barInit
 * 功能描述： 设置刚进入bar时初始值
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/19	     V1.0	  Ervin	      创建
 ***********************************************************************/

static int8_t barInit(){
	if(g_currentPage.InfMode != nBar)
		return -1;
	int i;
	OLED_SetPos(BarXStart,BarYStart);
	for(i=0;i<=g_currentPage.data;i++)
		WriteDat(0xFF);
	return 0;
}


/**********************************************************************
 * 函数名称： showbarFrame
 * 功能描述： 数值条边框显示函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/19	     V1.0	  Ervin	      创建
 ***********************************************************************/
static void showbarFrame(){
	uint8_t i;
	OLED_SetPos(BarXStart-2,BarYStart);
	for(i=0;i<=BarLength+3;i++){
		if(i==0||i==1||i==BarLength+2||i==BarLength+3)
			WriteDat(0xFF);
		else
			WriteDat(0xC3);
	}

}

/**********************************************************************
 * 函数名称： clearBar
 * 功能描述： 擦除整个Bar
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/23	     V1.0	  Ervin	      创建
 ***********************************************************************/
static void clearBar(){
	uint8_t i;
	OLED_SetPos(BarXStart-2,BarYStart);
	for(i=0;i<=BarLength+3;i++){
		WriteDat(0x00);
	}

}


/**********************************************************************
 * 函数名称： Int2String
 * 功能描述： 整型转字符串型
 * 输入参数： 数字
 * 输出参数： 字符串首地址
 * 返 回 值： 字符串首地址
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/19	     V1.0	  Ervin	      创建
 ***********************************************************************/
char* Int2String(int num,char *str)//10进制 
{
    int i = 0;//指示填充str 
    if(num<0)//如果num为负数，将num变正 
    {
        num = -num;
        str[i++] = '-';
    } 
    //转换 
    do
    {
        str[i++] = num%10+48;//取num最低位 字符0~9的ASCII码是48~57；简单来说数字0+48=48，ASCII码对应字符'0' 
        num /= 10;//去掉最低位    
    }while(num);//num不为0继续循环
    
    str[i] = '\0';
    
    //确定开始调整的位置 
    int j = 0;
    if(str[0]=='-')//如果有负号，负号不用调整 
    {
        j = 1;//从第二位开始调整 
        ++i;//由于有负号，所以交换的对称轴也要后移1位 
    }
    //对称交换 
    for(;j<i/2;j++)
    {
        //对称交换两端的值 其实就是省下中间变量交换a+b的值：a=a+b;b=a-b;a=a-b; 
        str[j] = str[j] + str[i-1-j];
        str[i-1-j] = str[j] - str[i-1-j];
        str[j] = str[j] - str[i-1-j];
    } 
    
    return str;//返回转换后的值 
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
static void clearMainIcon(){
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
static void clearString(){
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
static void showString(){
	if(g_currentPage.InfMode == pSlide){
		uint8_t len = strlen(bottomStr[g_currentId-1][g_currentPage.data-1]);
		g_string_LenRes = len<<3;
		g_string_Xpos = ((Xres-g_string_LenRes)>>1) - 1;
		OLED_ShowStr(g_string_Xpos, g_string_Ypos, (unsigned char*)bottomStr[g_currentId-1][g_currentPage.data-1], g_stringSize);
	}
	else{
		uint8_t len = strlen(bottomStr[g_currentId-1][0]);
		g_string_LenRes = len<<3;
		g_string_Xpos = ((Xres-g_string_LenRes)>>1) - 1;
		OLED_ShowStr(g_string_Xpos, g_string_Ypos, (unsigned char*)bottomStr[g_currentId-1][0], g_stringSize);
	}
	
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
static void clearBottom(){
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
static void clearArrows(){
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
static void showArrows(){
	OLED_DrawBMP(0,6,ArrowBMPres,8,BMP_Left_16x16);//显示左箭头
	OLED_DrawBMP(Xres-ArrowBMPres,6,Xres,8,BMP_Right_16x16);//显示右箭头
}

/**********************************************************************
 * 函数名称： clearBackArrow
 * 功能描述： 清除返回箭头
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/17	     V1.0	  Ervin	      创建
 ***********************************************************************/
static void clearBackArrow(){
	uint8_t i, j;
	for(i=0; i<2; i++){
		OLED_SetPos(0,i);
		for(j=0;j<ArrowBMPres;j++)
			WriteDat(0x00);
	}
}

/**********************************************************************
 * 函数名称： showBackArrow
 * 功能描述： 子页面显示返回箭头
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/17	     V1.0	  Ervin	      创建
 ***********************************************************************/
static void showBackArrow(){
	OLED_DrawBMP(0,0,ArrowBMPres,2,BMP_Left_16x16);
}