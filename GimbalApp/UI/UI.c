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
	extern EventGroupHandle_t ActionEvent;
	extern MotorInf g_currentMotorInf;
	extern TaskHandle_t MotorPidTaskHandle;
	PageID id;
	float targetAngle = 0;
	while(1){
		xEventGroupWaitBits(ActionEvent, 1<<0, pdFALSE, pdFALSE, portMAX_DELAY);
		id = getCurrentpageId();
		//printf("%f,%f\n", motorInf.angle, targetAngle);
		switch(id){
			case On:{
				if(g_currentMotorInf.angle>-_3_PI_8){
					targetAngle = 0;
					xQueueSend(TargetAngleQueueHandle, &targetAngle, portMAX_DELAY);
					clearPage();
					setCurrentpage(Off);
					showPage();
					vTaskDelay(pdMS_TO_TICKS(1000));
				}
				break;
			}

			case Off:{
				if(g_currentMotorInf.angle<-_PI_8){
					targetAngle = -_PI_2;
					xQueueSend(TargetAngleQueueHandle, &targetAngle, portMAX_DELAY);
					clearPage();
					setCurrentpage(On);
					showPage();
					vTaskDelay(pdMS_TO_TICKS(1000));
				}
				break;
			}
			
			case Bright1:{
				if(g_currentMotorInf.angle<_3_PI_8){
					targetAngle = 0;
					xQueueSend(TargetAngleQueueHandle, &targetAngle, portMAX_DELAY);
					clearPage();
					setCurrentpage(Bright2);
					showPage();
					vTaskDelay(pdMS_TO_TICKS(1000));
				}
				break;
			}
			
			case Bright2:{
				if(g_currentMotorInf.angle>_PI_8){
					targetAngle = _PI_2;
					xQueueSend(TargetAngleQueueHandle, &targetAngle, portMAX_DELAY);
					clearPage();
					setCurrentpage(Bright1);
					showPage();
					vTaskDelay(pdMS_TO_TICKS(1000));
				}
				else if(g_currentMotorInf.angle<-_PI_8){
					targetAngle = -_PI_2;
					xQueueSend(TargetAngleQueueHandle, &targetAngle, portMAX_DELAY);
					clearPage();
					setCurrentpage(Bright3);
					showPage();
					vTaskDelay(pdMS_TO_TICKS(1000));
				}
				break;
			}
			
			case Bright3:{
				if(g_currentMotorInf.angle>-_3_PI_8){
					targetAngle = 0;
					xQueueSend(TargetAngleQueueHandle, &targetAngle, portMAX_DELAY);
					clearPage();
					setCurrentpage(Bright2);
					showPage();
					vTaskDelay(pdMS_TO_TICKS(1000));
				}
				break;
			}
			
			//PI到 -PI映射到0-100
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
	extern EventGroupHandle_t UIResponseEvent;
	extern EventGroupHandle_t ActionEvent;
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
					PageIn();
					xEventGroupSetBits(ActionEvent, 1<<0);
				}
				//成功后延迟0.5秒并清除所有位
				vTaskDelay(pdMS_TO_TICKS(500));
				xEventGroupClearBits(UIResponseEvent, 1<<0|1<<1|1<<2|1<<3|1<<4);
			}
		}
		//若为子页面
		else{
			xEventGroupWaitBits(UIResponseEvent, 1<<0|1<<1|1<<2|1<<3, pdTRUE, pdFALSE, portMAX_DELAY);
			PageOut();
			xEventGroupClearBits(ActionEvent, 1<<0);
			//成功后延迟0.5秒并清除所有位
			vTaskDelay(pdMS_TO_TICKS(500));
			xEventGroupClearBits(UIResponseEvent, 1<<0|1<<1|1<<2|1<<3);
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

