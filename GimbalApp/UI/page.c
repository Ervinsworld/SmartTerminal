#include "page.h"
#include "stm32f4xx_hal.h"
#include "driver_oled.h"
#include <string.h>
#include "FreeRTOS.h"
#include "Icons.c"


/************					页面相关表				************/
//父子页面关系表，二维数组每行的第一个元素是父页面，其余是子页面，索引号和UI_page结构体的data对应
const uint8_t PageMapTable[FatherPageNum][MaxSonPageNum] = 
{
	{Light, Off, On},
	{Mode, Bright1, Bright2, Bright3},
	{Window, Bar},
};


//页面底部的字符串信息，对于pSlide模式的页面，其索引号和UI_page结构体的data对应
const char* bottomStr[PageNum][3] = 
{
	{"Light", "", ""},
	{"OFF", "", ""},
	{"ON", "", ""},
	{"Mode", "", ""},
	{"Low", "", ""},
	{"Medium", "", ""},
	{"High", "", ""},
	{"Brightness", "", ""},
	{"Free", "Ratchet", "Fixed"},
	{"Block", "Car"},
};

UIPage UIpages[PageNum] = 
{	
{pStatic, 0, 1, BMP_LIGHT_48x48},//data值1表示灯光关闭，2表示灯光开启
{nStatic, 0, 0, BMP_OFF_48x48},
{nStatic, 0, 0, BMP_ON_48x48},
{pStatic, 0, 1, BMP_Sunny_48x48},//data值1，2，3代表亮度1，2，3
{nStatic, 0, 0, BMP_BRIGHT1_48x48},
{nStatic, 0, 0, BMP_BRIGHT2_48x48},
{nStatic, 0, 0, BMP_BRIGHT3_48x48},
{pStatic, 0, 0, BMP_WINDOW_48x48},//data存储bar的值，0-100，精度1
{pSlide, 3, 0, BMP_MOTOR_48x48},
{pSlide, 2, 0, BMP_SWITCH_48x48},
{nDynamic, 0, 0, NULL},
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
	uint8_t id = getCurrentpageId();
	UIPage page = getCurrentpage();
	//若该页是被动状态，则无法执行翻页操作
	if((page.InfMode==nStatic) || (page.InfMode==nDynamic))
		return -1;
	do{
		if(id==PageNum){
			id = 1;
		}
		else{
			id++;
		}
	}while((UIpages[id-1].InfMode==nStatic) || (UIpages[id-1].InfMode==nDynamic));
	clearPage();
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
	uint8_t id = getCurrentpageId();
	UIPage page = getCurrentpage();
	//若该页是被动状态，则无法执行翻页操作
	if((page.InfMode==nStatic) || (page.InfMode==nDynamic))
		return -1;
	do{
		if(id==1){
			id = PageNum;
		}
		else{
			id--;
		}

	}while((UIpages[id-1].InfMode==nStatic) || (UIpages[id-1].InfMode==nDynamic));
	clearPage();
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
	if(data == num-1){
		data = 0;
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
	if(data == 0){
		data = num-1;
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
	int i;
	clearPage();
	if(g_currentId == Window){
		vTaskDelay(OLED_DELAY);
		setCurrentpage(Bar);
		showbarFrame();
		showbardata();
		return 0;
	}
	for(i=0; i<FatherPageNum; i++){
		//查询页面映射关系表
		if(PageMapTable[i][0] == g_currentId){
			setCurrentpage(PageMapTable[i][g_currentPage.data]);
			showPage();
			return 0;
		}
	}
	return -1;
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
	int i, j;
	//若为Bar页面
	if(g_currentId == Bar){
		vTaskDelay(OLED_DELAY);
		clearBar();
		vTaskDelay(OLED_DELAY);
		clearString();
		vTaskDelay(OLED_DELAY);
		setCurrentpage(Window);
		showPage();
		return 0;
	}
	//若为其他页面
	for(i=0; i<FatherPageNum; i++){
		for(int j=1; j<MaxSonPageNum; j++){
			if(PageMapTable[i][j] == getCurrentpageId()){
				clearPage();
				setCurrentpage(PageMapTable[i][0]);
				showPage();
				return 0;
			}
		}
	}
	return -1;
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
	if(g_currentPage.InfMode == pSlide && data >= g_currentPage.slideNum)
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
	if(g_currentPage.InfMode == pSlide)
		showArrows();
	else if(g_currentPage.InfMode == nStatic||g_currentPage.InfMode == nDynamic)
		showBackArrow();
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
	clearString();
	if(g_currentPage.InfMode == pSlide)
		clearArrows();
	else if(g_currentPage.InfMode == nStatic||g_currentPage.InfMode == nDynamic)
		clearBackArrow();
}

/**********************************************************************
 * 函数名称： showbar
 * 功能描述： 数值条显示函数
 * 输入参数： 数值条的值，从0-100
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/19	     V1.0	  Ervin	      创建
 ***********************************************************************/

int8_t showbar(){
	if(g_currentId != Bar)
		return -1;
	static uint8_t last_value = 0;
	int8_t error;
	uint8_t i, j;
	error = g_currentPage.data - last_value;
	if(error>0){
		for(i=0; i<BarWidth; i++){
			OLED_SetPos(BarXStart+last_value,BarYStart+i);
			for(j=0;j<error;j++)
				WriteDat(0xFF);
		}
	}
	else if(error<0){
		for(i=0; i<BarWidth; i++){
			OLED_SetPos(BarXStart+g_currentPage.data,BarYStart+i);
			for(j=0;j<-error;j++)
				WriteDat(0x81);
		}
	}
	last_value = g_currentPage.data;
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
void showbarFrame(){
	uint8_t i, j;
	for(i=0; i<BarWidth; i++){
		OLED_SetPos(BarXStart,BarYStart+i);
		for(j=0;j<BarLength;j++){
			if(j==0||j==BarLength-1)
				WriteDat(0xFF);
			else
				WriteDat(0x81);
		}
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
void clearBar(){
	uint8_t i, j;
	for(i=0; i<BarWidth; i++){
		OLED_SetPos(BarXStart,BarYStart+i);
		for(j=0;j<BarLength;j++){
			WriteDat(0x00);
		}
	}
}

/**********************************************************************
 * 函数名称： showbardata
 * 功能描述： 数值条底部数值的显示函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0为成功，-1为失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/19	     V1.0	  Ervin	      创建
 ***********************************************************************/
int8_t showbardata(){
	char s[7] = {0};
	char result_s[7] = " ";
	if(g_currentId!=Bar)
		return -1;
	Int2String(g_currentPage.data, s);
	//给转换完成的数字前后都加上一个空格“ ”
	strcat(s, " ");
	strcat(result_s, s);
	//计算字符串显示参数并显示字符
	uint8_t len = strlen(result_s);
	g_string_LenRes = len<<3;
	g_string_Xpos = ((Xres-g_string_LenRes)>>1) - 1;
	OLED_ShowStr(g_string_Xpos, g_string_Ypos, (unsigned char*)result_s, g_stringSize);
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
	if(g_currentPage.InfMode == pSlide){
		uint8_t len = strlen(bottomStr[g_currentId-1][g_currentPage.data]);
		g_string_LenRes = len<<3;
		g_string_Xpos = ((Xres-g_string_LenRes)>>1) - 1;
		OLED_ShowStr(g_string_Xpos, g_string_Ypos, (unsigned char*)bottomStr[g_currentId-1][g_currentPage.data], g_stringSize);
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
void clearBackArrow(){
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
void showBackArrow(){
	OLED_DrawBMP(0,0,ArrowBMPres,2,BMP_Left_16x16);
}