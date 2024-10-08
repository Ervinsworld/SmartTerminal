#include "driver_oled.h"
#include "driver_joystick.h"
#include "page.h"
#include "UI.h"
#include "driver_motorComm.h"
#include "pid.h"
#include "Motor.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core

extern TaskHandle_t MotorPidTaskHandle;
extern QueueHandle_t TargetAngleQueueHandle;
extern QueueHandle_t AngleDiffQueueHandle;
extern EventGroupHandle_t UIResponseEvent;
extern EventGroupHandle_t MqttNotifyEvent;
extern SemaphoreHandle_t UIActionSemaphore;
extern SemaphoreHandle_t MotorPidSemaphore;

void mqtt_switchNotify(float oldTargetAngle, float diffAngle, PageID targetPage, int waitTime);
void mqtt_barNotify(uint8_t value);

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
	PageID id;
	float targetAngle = 0;
	float angleDiff = 0;
	while(1){
		//等待按下按键进入子页面，该信号量决定这整个任务的启停
		xSemaphoreTake(UIActionSemaphore, portMAX_DELAY);
		xQueuePeek(AngleDiffQueueHandle, &angleDiff, portMAX_DELAY);
		id = getCurrentpageId();
		//printf("%f,%f\n", getMotorAngle(), targetAngle);
		
		//若为nStatic页面
		if(getCurrentpage().InfMode == nStatic){	
			switch(id){
				case On:{
					if(getMotorAngle()>-_3_PI_4 - angleDiff)
						mqtt_switchNotify(0, angleDiff, Off, 1000);
					break;
				}

				case Off:{
					if(getMotorAngle()<-_PI_4 - angleDiff)
						mqtt_switchNotify(-_PI, angleDiff, On, 1000);
					break;
				}
				
				case Bright1:{
					if(getMotorAngle()<_3_PI_4 - angleDiff)
						mqtt_switchNotify(0, angleDiff, Bright2, 1000);
					break;
				}
				
				case Bright2:{
					if(getMotorAngle()>_PI_4 - angleDiff)
						mqtt_switchNotify(_PI, angleDiff, Bright1, 1000);
					else if(getMotorAngle()<-_PI_4 - angleDiff)
						mqtt_switchNotify(-_PI, angleDiff, Bright3, 1000);
					break;
				}
				
				case Bright3:{
					if(getMotorAngle()>-_3_PI_4 - angleDiff)
						mqtt_switchNotify(0, angleDiff, Bright2, 1000);
					break;
				}
				default: break;	
			}
		}
		else if(getCurrentpage().InfMode == nBar){
			//PI到 -PI映射到0-100
			UIPage page = getCurrentpage();
			float value = page.data;//bar显示的值，0-100
			float minAngle = _PI - angleDiff; //最小边界值
			float maxAngle = -_PI - angleDiff;  //最大边界值
			//将电机角度值转换为value值
			value = -(getMotorAngle() + angleDiff)*(50/_PI) + 50;
			if(value>=-5&&value<105){
				//确保pid任务暂停，电机断电
				xSemaphoreTake(MotorPidSemaphore, 0);
				SendMessage2Motor(0 ,motorID);
				uint8_t realvalue = 0;
				if(value<0)
					realvalue = 0;
				else if(value > 100)
					realvalue = 100;
				else
					realvalue = value;
				setCurrentPagedata(realvalue);
				
				switch(id){
					case LightBar:mqtt_barNotify(realvalue);break;
					case BriBar: setScreenBri((float)getCurrentpage().data*2.55);break;
					case IntensBar:setIntens((float)getCurrentpage().data*0.4+20);break;
					default : break;
					
				}
				showbar();
				//clearString();
				showBottomData(getCurrentpage().data);
			}
			else if(value<-5){
				xQueueOverwrite(TargetAngleQueueHandle, &minAngle);
				xSemaphoreGive(MotorPidSemaphore);
			}
			else if(value>=105){
				xQueueOverwrite(TargetAngleQueueHandle, &maxAngle);
				xSemaphoreGive(MotorPidSemaphore);
			}
		}
		xSemaphoreGive(UIActionSemaphore);
		//printf("%f\n", targetAngle);
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
	float diffAngle = 0;
	UI_Init();//初始化UI
	while(1){
		InterfaceMode mode = getCurrentpage().InfMode;
		//若为父页面
		if((mode == pStatic)||(mode == pSlide)){
			UIResponseEventbit = xEventGroupWaitBits(UIResponseEvent, 1<<0|1<<1|1<<2|1<<3|1<<4, 
			pdTRUE, pdFALSE, portMAX_DELAY);
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
					xQueuePeek(TargetAngleQueueHandle, &targetAngle, 0);	
					id = getCurrentpageId();
					//若为特定页面则使能电机pid
					//vTaskDelay(3000);
					PageIn();
					//vTaskDelay(3000);
					id = getCurrentpageId();
					//vTaskDelay(3000);
					//printf("current id1: %d", id);
					if(getCurrentpage().InfMode == nStatic){
						switch(id){
							case On: diffAngle = -_PI - targetAngle;	break;
							case Off: diffAngle = 0 - targetAngle;	break;
							case Bright1: diffAngle = _PI - targetAngle;	break;
							case Bright2: diffAngle = 0 - targetAngle;	break;
							case Bright3: diffAngle = -_PI - targetAngle;	break;
							default:break;	
						}
					}
					else if(getCurrentpage().InfMode == nBar){
						//vTaskDelay(1000);
						diffAngle = -(float)getCurrentpage().data*_PI/50 + _PI - targetAngle;
						//printf("bar diffangle is %f:\n",diffAngle);
					}
					//printf("current id2: %d", id);
					xQueueOverwrite(AngleDiffQueueHandle, &diffAngle);
					//vTaskDelay(pdMS_TO_TICKS(2000));
					xSemaphoreGive(UIActionSemaphore);
					//printf("%f, %f\n", targetAngle, getMotorAngle());	
				}
				//成功后延迟并清除所有位
				//printf("bit:%d\n", UIResponseEventbit);
				vTaskDelay(pdMS_TO_TICKS(700));
				xEventGroupClearBits(UIResponseEvent, 1<<0|1<<1|1<<2|1<<3|1<<4);
			}
			
		}
		//若为子页面,摇杆移动任意方向或按下皆可退出到父页面操作
		else{
			xEventGroupWaitBits(UIResponseEvent, 1<<0|1<<1|1<<2|1<<3, pdTRUE, pdFALSE, portMAX_DELAY);
			//xQueueOverwrite(TargetAngleQueueHandle, &targetAngle);
			xSemaphoreTake(UIActionSemaphore, portMAX_DELAY);//停止动作执行任务
			
			if(getCurrentpage().InfMode == nBar){
				//Bar页面需恢复电机使能
				xSemaphoreGive(MotorPidSemaphore);
				//发送当前角度为目标角度已备退出使用
				targetAngle = getMotorAngle();
				xQueueOverwrite(TargetAngleQueueHandle, &targetAngle);
			}
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
 * 函数名称： UIPrint_Task
 * 功能描述： UI信息的打印测试函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/4/6	     V1.0	  Ervin	      创建
 ***********************************************************************/

void UIPrint_Task(void *params){
	while(1){
		printf("%d\n", getCurrentpageId());
		vTaskDelay(pdMS_TO_TICKS(50));
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
	PageInit();
	vTaskDelay(1000);
}


void mqtt_switchNotify(float oldTargetAngle, float diffAngle, PageID targetPage, int waitTime){
	float targetAngle = oldTargetAngle - diffAngle;
	xQueueOverwrite(TargetAngleQueueHandle, &targetAngle);
	sonPageSwitch(targetPage);
	if(targetPage == On || targetPage == Off)
		xEventGroupSetBits(MqttNotifyEvent, 1<<0);
	else if(targetPage == Bright1 || targetPage == Bright2 || targetPage == Bright3)
		xEventGroupSetBits(MqttNotifyEvent, 1<<1);
	vTaskDelay(pdMS_TO_TICKS(waitTime));
}


void mqtt_barNotify(uint8_t value){
	static uint8_t lastvalue = 0;
	if(lastvalue!=value){
		xEventGroupSetBits(MqttNotifyEvent, 1<<2);
		lastvalue = value;
	}
}

