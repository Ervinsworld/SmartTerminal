#include "page.h"
#include "stm32f4xx_hal.h"
#include "driver_oled.h"
#include <string.h>
#include "FreeRTOS.h"
#include "Icons.c"

#define Xres 128  //oled x�ֱ���
#define Yres 64	  //oled y�ֱ���
#define MainBMPres 48   //��ͼ������ֵ
#define ArrowBMPres 16  //��ͷͼ������ֵ��ֻ�л���ģʽӵ�м�ͷͼ��

#define PageNum 13
#define FatherPageNum 5 //��ҳ�������
#define MaxSonPageNum 4	//������ҳ�������

#define BarXStart 14
#define BarYStart 3
#define BarLength 100
//#define BarWidth  1

//����oled�����ܺ�ʱ����oled�����ܼ�ʱ����Ҫʹ�ö�����ʱ��������Է�ֹ��֡;�ú��vTaskDelay���ã���λΪtick
#define OLED_DELAY 50	

/************					ҳ����ر�				************/
//����ҳ���ϵ����ά����ÿ�еĵ�һ��Ԫ���Ǹ�ҳ�棬��������ҳ�棬�����ź�UI_page�ṹ���data��Ӧ
const uint8_t PageMapTable[FatherPageNum][MaxSonPageNum] = 
{
	{Light1, Off, On},
	{Light2, Bright1, Bright2, Bright3},
	{Light3, LightBar},
	{Settings, BriBar, IntensBar},
};

//ҳ��ײ����ַ�����Ϣ������ֵΪ.h�ļ���ҳ���ID
//������ע�⣺����pSlideģʽ��ҳ�棬��������һ��Ҫ�ͺ�UI_page�ṹ���data��Ӧ
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

//����ֵΪ.h�ļ���ҳ���ID
UIPage UIpages[PageNum] = 
{	
{pStatic, 0, 1, BMP_LIGHT1_48x48},//dataֵ1��ʾ�ƹ�رգ�2��ʾ�ƹ⿪��
{nStatic, 0, 0, BMP_OFF_48x48},
{nStatic, 0, 0, BMP_ON_48x48},
{pStatic, 0, 1, BMP_LIGHT2_48x48},//dataֵ1��2��3��������1��2��3
{nStatic, 0, 0, BMP_BRIGHT1_48x48},
{nStatic, 0, 0, BMP_BRIGHT2_48x48},
{nStatic, 0, 0, BMP_BRIGHT3_48x48},
{pStatic, 0, 1, BMP_LIGHT3_48x48},//data��Ϊ1
{pSlide, 2, 1, BMP_SWITCH_48x48},
{pSlide, 2, 1, BMP_SETTINGS_48x48},
{nBar, 0, 0, NULL},//lightBarҳ�棬data�洢bar��ֵ��0-100������1, ����Ĭ��Ϊ0
{nBar, 0, 100,NULL}, //BriBarҳ�棬data�洢bar��ֵ��0-100������1, ����Ĭ��Ϊ100
{nBar, 0, 50,NULL}, //DampingBarҳ�棬data�洢bar��ֵ��0-100������1, ����Ĭ��Ϊ50
};


//��ǰҳ��Ľṹ������͵�ǰҳ���id
UIPage g_currentPage;
PageID g_currentId;

//��ͼ��λ�ò���
static uint8_t g_mainIcon_Xstart;
static uint8_t g_mainIcon_XEnd;
static uint8_t g_mainIcon_Ystart;
static uint8_t g_mainIcon_YEnd;

//�ײ���Ϣ�ַ�����Ϣ����
static uint8_t g_stringSize = 2;//�ַ���С�㶨Ϊ2
static uint8_t g_string_Ypos = 6;//�ַ�����ʾy����㶨Ϊ6
static uint8_t g_string_Xpos;
static uint8_t g_string_LenRes;

//��������
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
 * �������ƣ� PageInit
 * ���������� Pageҳ�濪����ʼ��
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      ����
 ***********************************************************************/
void PageInit(){
	OLED_Init();
	OLED_CLS();
	PagesInfInit();
	setCurrentpage(Light1);//��switchpage��Ϊ������ʼҳ��
	showPage();
}

/**********************************************************************
 * �������ƣ� PageDown
 * ���������� ����һҳ
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0��ʾ�ɹ���-1��ʾʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/7	     V1.0	  Ervin	      ����
 ***********************************************************************/
int8_t PageDown(){
	uint8_t id = g_currentId;
	UIPage page = g_currentPage;
	//����ҳ�Ǳ���״̬�����޷�ִ�з�ҳ����
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
 * �������ƣ� PageUp
 * ���������� ����һҳ
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0��ʾ�ɹ���-1��ʾʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/7      V1.0	  Ervin	      ����
 ***********************************************************************/
int8_t PageUp(){
	uint8_t id = g_currentId;
	UIPage page = g_currentPage;
	//����ҳ�Ǳ���״̬�����޷�ִ�з�ҳ����
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
 * �������ƣ� SlideRight
 * ���������� ���һ���
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0��ʾ�ɹ���-1��ʾʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/7	     V1.0	  Ervin	      ����
 ***********************************************************************/
int8_t SlideRight(){
	//uint8_t id = getCurrentpageId();
	UIPage page = getCurrentpage();
	//����ҳ���ǻ���ҳ�棬���޷�ִ�л���
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
 * �������ƣ� SlideLeft
 * ���������� ���һ���
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� 0��ʾ�ɹ���-1��ʾʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/7	     V1.0	  Ervin	      ����
 ***********************************************************************/
int8_t SlideLeft(){
	//uint8_t id = getCurrentpageId();
	UIPage page = getCurrentpage();
	//����ҳ���ǻ���ҳ�棬���޷�ִ�л���
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
 * �������ƣ� PageIn
 * ���������� ������ҳ��
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� -1Ϊʧ��,���򷵻�0
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      ����
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
 * �������ƣ� PageOut
 * ���������� ���ظ�ҳ��
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� -1Ϊʧ�ܣ�0Ϊ�ɹ�
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      ����
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
 * �������ƣ� sonPageSwitch
 * ���������� ������ҳ����л�
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� -1Ϊʧ�ܣ�0Ϊ�ɹ�
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      ����
 ***********************************************************************/
int8_t sonPageSwitch(PageID targetPage){
	int i, j;
	int currentPageFaId = -1, targetPageFaId = -1;//��¼��ҳ��͸�ҳ���id
	uint8_t targetPageNum;
	//����ҳ�治��nStatic������-1
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
	//�ж�ҳ���Ƿ��й�ͬ��ҳ�棬�����򷵻�-1
	if(currentPageFaId!=targetPageFaId)
		return -1;
	//�޸ĸ�ҳ���dataֵ
	setPagedata(PageMapTable[targetPageFaId][0], targetPageNum);
	//ִ�л�ҳ����
	clearString();
	//clearMainIcon();
	setCurrentpage(targetPage);
	showString();
	OLED_DrawBMP(g_mainIcon_Xstart,g_mainIcon_Ystart,g_mainIcon_XEnd,g_mainIcon_YEnd,g_currentPage.mainBMP);
	return 0;
}

/**********************************************************************
 * �������ƣ� setScreenBri
 * ���������� ����ҳ������
 * ��������� 0x00~0xff
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      ����
 ***********************************************************************/
void setScreenBri(uint8_t value){
	//g_currentPage = UIpages[id];
	WriteCmd(0x81); //--set contrast control register
    WriteCmd(value); //���ȵ��� 0x00~0xff
}

/**********************************************************************
 * �������ƣ� setCurrentpage
 * ���������� ���õ�ǰҳ�棬д��ȫ�ֱ���
 * ��������� pageID
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      ����
 ***********************************************************************/
void setCurrentpage(PageID id){
	//g_currentPage = UIpages[id];
	memcpy(&g_currentPage, &UIpages[id-1], sizeof(g_currentPage));
	g_currentId = id;
}

/**********************************************************************
 * �������ƣ� getCurrentpage
 * ���������� ��ȡ��ǰҳ��ṹ��
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� UIPage
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      ����
 ***********************************************************************/
UIPage getCurrentpage(){
	return g_currentPage;
}

/**********************************************************************
 * �������ƣ� getCurrentpageId
 * ���������� ��ȡ��ǰҳ��ṹ��Id
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� PageID
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      ����
 ***********************************************************************/
PageID getCurrentpageId(){
	return g_currentId;
}

/**********************************************************************
 * �������ƣ� setCurrentPagedata
 * ���������� ���õ�ǰҳ�������ֵ
 * ��������� data
 * ��������� ��
 * �� �� ֵ�� -1��ʾʧ�ܣ�0��ʾ�ɹ�
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/7	     V1.0	  Ervin	      ����
 ***********************************************************************/
int8_t setCurrentPagedata(uint8_t data){
	//�ж��趨ֵ�Ƿ񳬹�������ֵ�����������������������-1
	if(g_currentPage.InfMode == pSlide && data > g_currentPage.slideNum)
		return -1;
	else{
		g_currentPage.data = data;
		UIpages[g_currentId-1].data = data;
	}
	return 0;
}

/**********************************************************************
 * �������ƣ� setPagedata
 * ���������� ����ָ��ҳ�������ֵ
 * ��������� id, data
 * ��������� ��
 * �� �� ֵ�� -1��ʾʧ�ܣ�0��ʾ�ɹ�
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/7	     V1.0	  Ervin	      ����
 ***********************************************************************/
int8_t setPagedata(uint8_t id, uint8_t data){
	//�ж��趨ֵ�Ƿ񳬹�������ֵ�����������������������-1
	UIPage page = UIpages[id-1];
	if(page.InfMode == pSlide && data >= page.slideNum)
		return -1;
	else 
		UIpages[id-1].data = data;
	return 0;
}

/**********************************************************************
 * �������ƣ� getPagedata
 * ���������� ��ȡָ��ҳ�������ֵ
 * ��������� id
 * ��������� ��
 * �� �� ֵ�� data�� -1��ʾ����
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/9/4	     V1.0	  Ervin	      ����
 ***********************************************************************/
int8_t getPagedata(uint8_t id){
	//�ж��趨ֵ�Ƿ񳬹�������ֵ�����������������������-1
	UIPage page = UIpages[id-1];
	if(page.InfMode == pSlide)
		return -1;
	return page.data;
};


/**********************************************************************
 * �������ƣ� showBottomData
 * ���������� ��ֵ���ײ���ֵ����ʾ����
 * ��������� int
 * ��������� ��
 * �� �� ֵ�� 0Ϊ�ɹ���-1Ϊʧ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/19	     V1.0	  Ervin	      ����
 ***********************************************************************/
int8_t showBottomData(int8_t data){
	char s[7] = {0};
	char result_s[7] = " ";
	if(g_currentPage.InfMode!=nBar)
		return -1;
	Int2String(data, s);
	//��ת����ɵ�����ǰ�󶼼���һ���ո� ��
	strcat(s, " ");
	strcat(result_s, s);
	//�����ַ�����ʾ��������ʾ�ַ�
	uint8_t len = strlen(result_s);
	g_string_LenRes = len<<3;
	g_string_Xpos = ((Xres-g_string_LenRes)>>1) - 1;
	OLED_ShowStr(g_string_Xpos, g_string_Ypos, (unsigned char*)result_s, g_stringSize);
	return 0;
}


/**********************************************************************
 * �������ƣ� showbar
 * ���������� ��ֵ����ʾ����
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/19	     V1.0	  Ervin	      ����
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
		//�ж��ϴε�ֵ������forѭ���Ŀ�ʼ�ͽ���ֵ���Խ�ֹɾ��bar�����ұ߿�
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
 * �������ƣ� PageInfInit
 * ���������� ���㲢��ʼ��Page��ʾ�����ȫ�ֱ���
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      ����
 ***********************************************************************/
static void PagesInfInit(){
	//������ͼ��λ�ã���֤��ͼ���ڻ�������
	g_mainIcon_Xstart = ((Xres-MainBMPres)>>1) - 1;
	g_mainIcon_XEnd = g_mainIcon_Xstart + MainBMPres;
	g_mainIcon_Ystart = 0;
	g_mainIcon_YEnd = (g_mainIcon_Ystart + MainBMPres)>>3;
	
	//�ײ���Ϣ�ַ���λ�ò���,��Ϊ��ֵ
	g_string_Ypos = 6;//�ַ�����ʾy����㶨Ϊ6
	g_stringSize = 2;//�ַ���С�㶨Ϊ2
}

static UIPage getPage(PageID id){
	return UIpages[id-1];
}

static int8_t getSonPage(PageID faPage){
	int i;
	for(i=0; i<FatherPageNum; i++){
	//��ѯҳ��ӳ���ϵ��
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
 * �������ƣ� showPage
 * ���������� ҳ����ʾ
 * ��������� UIPage�ṹ�壬������
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      ����
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
	//��Ϊ����ģʽ����ʾ��ͷ
	else if(g_currentPage.InfMode == pSlide)
		showArrows();
	//��Ϊ��̬ģʽ����ʾ���ؼ�ͷ
	else if(g_currentPage.InfMode == nStatic)
		showBackArrow();
	OLED_DrawBMP(g_mainIcon_Xstart,g_mainIcon_Ystart,g_mainIcon_XEnd,g_mainIcon_YEnd,g_currentPage.mainBMP);
	showString();
	//OLED_DrawBMP(39,0,87,6,BMP_Camera_48x48);
}

/**********************************************************************
 * �������ƣ� clearPage
 * ���������� �����ҳ���ݣ�ȫ�����̫��ʱ
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      ����
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
 * �������ƣ� barInit
 * ���������� ���øս���barʱ��ʼֵ
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/19	     V1.0	  Ervin	      ����
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
 * �������ƣ� showbarFrame
 * ���������� ��ֵ���߿���ʾ����
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/19	     V1.0	  Ervin	      ����
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
 * �������ƣ� clearBar
 * ���������� ��������Bar
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/23	     V1.0	  Ervin	      ����
 ***********************************************************************/
static void clearBar(){
	uint8_t i;
	OLED_SetPos(BarXStart-2,BarYStart);
	for(i=0;i<=BarLength+3;i++){
		WriteDat(0x00);
	}

}


/**********************************************************************
 * �������ƣ� Int2String
 * ���������� ����ת�ַ�����
 * ��������� ����
 * ��������� �ַ����׵�ַ
 * �� �� ֵ�� �ַ����׵�ַ
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/19	     V1.0	  Ervin	      ����
 ***********************************************************************/
char* Int2String(int num,char *str)//10���� 
{
    int i = 0;//ָʾ���str 
    if(num<0)//���numΪ��������num���� 
    {
        num = -num;
        str[i++] = '-';
    } 
    //ת�� 
    do
    {
        str[i++] = num%10+48;//ȡnum���λ �ַ�0~9��ASCII����48~57������˵����0+48=48��ASCII���Ӧ�ַ�'0' 
        num /= 10;//ȥ�����λ    
    }while(num);//num��Ϊ0����ѭ��
    
    str[i] = '\0';
    
    //ȷ����ʼ������λ�� 
    int j = 0;
    if(str[0]=='-')//����и��ţ����Ų��õ��� 
    {
        j = 1;//�ӵڶ�λ��ʼ���� 
        ++i;//�����и��ţ����Խ����ĶԳ���ҲҪ����1λ 
    }
    //�Գƽ��� 
    for(;j<i/2;j++)
    {
        //�Գƽ������˵�ֵ ��ʵ����ʡ���м��������a+b��ֵ��a=a+b;b=a-b;a=a-b; 
        str[j] = str[j] + str[i-1-j];
        str[i-1-j] = str[j] - str[i-1-j];
        str[j] = str[j] - str[i-1-j];
    } 
    
    return str;//����ת�����ֵ 
}

/**********************************************************************
 * �������ƣ� clearMainIcon
 * ���������� �����ͼ��48*48���ص�����
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      ����
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
 * �������ƣ� clearString
 * ���������� ����ײ��ַ�������Ĭ��sizeΪ2
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      ����
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
 * �������ƣ� showString
 * ���������� �����ַ���Ϣ����ȫ�ֱ���������ʾ�ײ��ַ���
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      ����
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
 * �������ƣ� clearBottom
 * ���������� ����ײ���������
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      ����
 ***********************************************************************/
static void clearBottom(){
	clearArrows();
	clearString();
}
/**********************************************************************
 * �������ƣ� clearArrows
 * ���������� ����ײ���ͷ����
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      ����
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
 * �������ƣ� showArrows
 * ���������� ����ģʽ��ҳ����ʾ���Ҽ�ͷ
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      ����
 ***********************************************************************/
static void showArrows(){
	OLED_DrawBMP(0,6,ArrowBMPres,8,BMP_Left_16x16);//��ʾ���ͷ
	OLED_DrawBMP(Xres-ArrowBMPres,6,Xres,8,BMP_Right_16x16);//��ʾ�Ҽ�ͷ
}

/**********************************************************************
 * �������ƣ� clearBackArrow
 * ���������� ������ؼ�ͷ
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/17	     V1.0	  Ervin	      ����
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
 * �������ƣ� showBackArrow
 * ���������� ��ҳ����ʾ���ؼ�ͷ
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/4/17	     V1.0	  Ervin	      ����
 ***********************************************************************/
static void showBackArrow(){
	OLED_DrawBMP(0,0,ArrowBMPres,2,BMP_Left_16x16);
}