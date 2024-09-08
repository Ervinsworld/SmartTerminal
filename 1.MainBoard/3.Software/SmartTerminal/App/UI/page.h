#ifndef __PAGE_H
#define __PAGE_H

#include "stm32f4xx_hal.h"

/*
ҳ������������֣�
	��ҳ��(��������,p��ͷ)���˵�����Ӧҡ�������ƶ���ѭ��ˢ�µ�ҳ��
			 ��������ҳ�������֣�
				- pStatic �������л������ҳ��
				- pSilde  �������޻�����ֻ�о�̬�ַ�������ҳ��
				
	��ҳ��(��������,n��ͷ)������������ҳ����¼������°�ť����������ҳ��
			 ��������ҳ�������֣�
				- nStatic  ֻ��ʾ���棬���汾��������޽���,����Ч���������nStaticҳ��Ŀ����л�
				- nBar 	   �н����ģ�����ʹ�ý���������
				- nDynamic �н����ģ�Ч����Ҫ����ʵ�֣�����Ϸ
*/


/************					ҳ�����ö����				************/
typedef enum
{
    pStatic = 0,
	pSlide= 1,
	nStatic = 2,
	nBar = 3,	  //��ֵ��ҳ��
	nDynamic = 4, //��̬ҳ�棬����Ϸҳ��
}InterfaceMode;

//ҳ����ţ���N��ҳ��,��c�ļ��в���ҳ��ľ�����Ϣ��ע����c�ļ��в���ʱҪһһ��Ӧ
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
	LightBar,  //Light3������ҳ��
	BriBar, //��Ļ������ҳ��
	IntensBar //������ҳ��
}PageID;

/************					ҳ����Ϣ�ṹ��				************/
typedef struct UI_page{
	InterfaceMode InfMode;
	const uint8_t slideNum; //ҳ��Ļ�������������ֻ��pSlideģʽҳ���У�����Ϊ0
	uint8_t data;//pSlideҳ�洫�ݻ�ҳ��Ϣ������ҳ�洫���Զ�����Ϣ
	const unsigned char* mainBMP;
}UIPage;


/************					ҳ���������					************/
//��ʼ������
void PageInit(void);

//����ת�Ʒ���
int8_t PageDown(void);
int8_t PageUp(void);
int8_t SlideRight(void);
int8_t SlideLeft(void);
int8_t PageIn(void);
int8_t PageOut(void);
int8_t sonPageSwitch(PageID targetPage);//��ҳ��Ļ�ҳ

//ȫ�ֱ���get/set����
void setCurrentpage(PageID id);
UIPage getCurrentpage(void);
PageID getCurrentpageId(void);
int8_t setCurrentPagedata(uint8_t data);
int8_t setPagedata(uint8_t id, uint8_t data);
int8_t getPagedata(uint8_t id);

//��ֵ����������
void showbarFrame(void);//��ʾ��ֵ�����
int8_t showbar(void);//����pagedata��ʾ��ֵ��
//int8_t showbardata(void);//��ʾ�ײ�����
int8_t showBottomData(int8_t data);
void clearBar(void);
int8_t barInit(void);

/*��Ļ��������*/
void setScreenBri(uint8_t value);

#endif
