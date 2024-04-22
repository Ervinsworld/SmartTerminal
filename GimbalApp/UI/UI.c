#include "driver_oled.h"
#include "driver_joystick.h"
#include "page.h"
#include "UI.h"
#include "driver_motorComm.h"
#include "Motor.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core

/**********************************************************************
 * 函数名称： UIAction_Task
 * 功能描述： 在子页面中执行相应的行为，主要是电机和屏幕的交互
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/19	     V1.0	  Ervin	      创建
 ***********************************************************************/

void UIAction_Task(void *params){ 
	extern QueueHandle_t TargetAngleQueueHandle;
	extern EventGroupHandle_t UIActionEvent;
	extern TaskHandle_t MotorPidTaskHandle;
	PageID id;
	float targetAngle = 0;
	while(1){
		//等待按下按键进入子页面，该事件决定这整个任务的启停
		xEventGroupWaitBits(UIActionEvent, 1<<0, pdFALSE, pdFALSE, portMAX_DELAY);
		id = getCurrentpageId();
		//printf("%f,%f\n", motorInf.angle, targetAngle);
		switch(id){
			case On:{
				if(g_currentMotorInf.angle>-_3_PI_8){
					targetAngle = 0;
					xQueueOverwrite(TargetAngleQueueHandle, &targetAngle);
					clearPage();
					setCurrentpage(Off);
					setPagedata(Light, 1);
					showPage();
					vTaskDelay(pdMS_TO_TICKS(1000));
				}
				break;
			}

			case Off:{
				if(g_currentMotorInf.angle<-_PI_8){
					targetAngle = -_PI_2;
					xQueueOverwrite(TargetAngleQueueHandle, &targetAngle);
					clearPage();
					setCurrentpage(On);
					setPagedata(Light, 2);
					showPage();
					vTaskDelay(pdMS_TO_TICKS(1000));
				}
				break;
			}
			
			case Bright1:{
				if(g_currentMotorInf.angle<_3_PI_8){
					targetAngle = 0;
					xQueueOverwrite(TargetAngleQueueHandle, &targetAngle);
					clearPage();
					setCurrentpage(Bright2);
					setPagedata(Mode, 2);
					showPage();
					vTaskDelay(pdMS_TO_TICKS(1000));
				}
				break;
			}
			
			case Bright2:{
				if(g_currentMotorInf.angle>_PI_8){
					targetAngle = _PI_2;
					xQueueOverwrite(TargetAngleQueueHandle, &targetAngle);
					clearPage();
					setCurrentpage(Bright1);
					setPagedata(Mode, 1);
					showPage();
					vTaskDelay(pdMS_TO_TICKS(1000));
				}
				else if(g_currentMotorInf.angle<-_PI_8){
					targetAngle = -_PI_2;
					xQueueOverwrite(TargetAngleQueueHandle, &targetAngle);
					clearPage();
					setCurrentpage(Bright3);
					setPagedata(Mode, 3);
					showPage();
					vTaskDelay(pdMS_TO_TICKS(1000));
				}
				break;
			}
			
			case Bright3:{
				if(g_currentMotorInf.angle>-_3_PI_8){
					targetAngle = 0;
					xQueueOverwrite(TargetAngleQueueHandle, &targetAngle);
					clearPage();
					setCurrentpage(Bright2);
					setPagedata(Mode, 2);
					showPage();
					vTaskDelay(pdMS_TO_TICKS(1000));
				}
				break;
			}
			
			//PI到 -PI映射到0-100
			/*
			case Window:{
				static int last_value = 0; 
				int value = ((_PI-g_currentMotorInf.angle)/_PI)*50;

				//printf("%d\n", value);
				printf("%d\n", getCurrentpage().data);
				if(value>=0&&value<=100&&(last_value!=value)){
					vTaskSuspend(MotorPidTaskHandle);
					SendMessage2Motor(0, motorID);
					setPagedata(value);
					showbar();
			    	clearString();
					showbardata();
				}
				last_value = value;
				break;
			}
			*/
			
			default:
				break;
		}

	}
}


/**********************************************************************
 * 函数名称： UI_Task
 * 功能描述： FreeRTOS的任务，用于页面的切换处理操作
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/

void UI_Task(void *params){
	EventBits_t UIResponseEventbit;
	PageID id;
	float targetAngle = 0;
	extern QueueHandle_t TargetAngleQueueHandle;
	extern EventGroupHandle_t UIResponseEvent;
	extern EventGroupHandle_t UIActionEvent;
	UI_Init();//初始化UI
	while(1){
		InterfaceMode mode = getCurrentpage().InfMode;
		//若为父页面
		if((mode == pStatic)||(mode == pSlide)){
			UIResponseEventbit = xEventGroupWaitBits(UIResponseEvent, 1<<0|1<<1|1<<2|1<<3|1<<4, pdTRUE, pdFALSE, portMAX_DELAY);
			if(UIResponseEventbit){
				//优先翻页
				if(UIResponseEventbit&(1<<2))
					PageDown();
				else if(UIResponseEventbit&(1<<3))
					PageUp();
				else if(UIResponseEventbit&(1<<1))
					SlideLeft();
				else if(UIResponseEventbit&(1<<0))
					SlideRight();
				else if(UIResponseEventbit&(1<<4)){
					
					//设置各子页面进入时的目标角度
					id = getCurrentpageId();
					PageIn();
					switch(id){
						case On: targetAngle = -_PI_2;break;
						case Off: targetAngle = 0;break;
						case Bright1: targetAngle = _PI_2;break;
						case Bright2: targetAngle = 0;break;
						case Bright3: targetAngle = -_PI_2;break;
						default:break;
					}
					xQueueOverwrite(TargetAngleQueueHandle, &targetAngle);
					vTaskDelay(pdMS_TO_TICKS(2000));
					xEventGroupSetBits(UIActionEvent, 1<<0);
					//printf("%f, %f\n", targetAngle, g_currentMotorInf.angle);	
				}
				//成功后延迟并清除所有位
				//printf("bit:%d\n", UIResponseEventbit);
				vTaskDelay(pdMS_TO_TICKS(1000));
				xEventGroupClearBits(UIResponseEvent, 1<<0|1<<1|1<<2|1<<3|1<<4);
			}
			
		}
		//若为子页面,摇杆移动任意方向或按下皆可退出到父页面操作
		else{
			xEventGroupWaitBits(UIResponseEvent, 1<<0|1<<1|1<<2|1<<3|1<<4, pdTRUE, pdFALSE, portMAX_DELAY);
			//xQueueOverwrite(TargetAngleQueueHandle, &targetAngle);
			xEventGroupClearBits(UIActionEvent, 1<<0);//停止动作执行任务
			PageOut();
			//printf("I am out\n");
			//成功后延迟0.5秒并清除所有位
			vTaskDelay(pdMS_TO_TICKS(500));
			xEventGroupClearBits(UIResponseEvent, 1<<0|1<<1|1<<2|1<<3|1<<4);//摇杆清0
			
		}
		//printf("%d\n", bit);
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
	setCurrentpage(Light);//将switchpage作为开机初始页面
	showPage();
}

