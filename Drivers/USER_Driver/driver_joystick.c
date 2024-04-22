#include "adc.h"
#include "gpio.h"
#include "driver_joystick.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups

static uint32_t ADC_Buffer[100];//用于储存ADC向DMA转运的数据，长度100
buttonState g_ButtonState = free;

//static buttonState g_ButtonState;
extern EventGroupHandle_t UIResponseEvent;
//extern QueueHandle_t ButtonQueueHandle;
/**********************************************************************
 * 函数名称： JoyStickInit
 * 功能描述： 摇杆初始化
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/3/14	     V1.0	  Ervin	      创建
 ***********************************************************************/
void JoyStickInit(){
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&ADC_Buffer, 100);
}

/**********************************************************************
 * 函数名称： JoyStickValueCal
 * 功能描述： 摇杆值计算，对于每个轴，将Buffer中的数据累加50次取平均值
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： struct JoystickerValue
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/3/14	     V1.0	  Ervin	      创建
 ***********************************************************************/

joystickValue JoyStickValueCal(){
	uint8_t i;
	uint32_t x, y;
	joystickValue value;
	for(i = 0,x =0,y=0; i < 100;)
	{
		x += ADC_Buffer[i++];
		y += ADC_Buffer[i++];
	}
	x /= 50;
	y /= 50;
	//由于摇杆安装方式，x, y对调
	value.xValue = y;
	value.yValue = x;
	return value;
}	

/**********************************************************************
 * 函数名称： JoystickButton_IRQ_Callback
 * 功能描述： 摇杆按键的中断回调函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 *  2024/4/7	     V1.0	  Ervin	      创建
 ***********************************************************************/

void JoystickButton_IRQ_Callback(void){
	if(HAL_GPIO_ReadPin(Button_GPIO_Port, Button_Pin)==GPIO_PIN_SET){
		g_ButtonState = free;
		//xQueueOverwriteFromISR(ButtonQueueHandle, &g_ButtonState, NULL);
	}	
	else{
		g_ButtonState = pressed;
		//xQueueOverwriteFromISR(ButtonQueueHandle, &g_ButtonState, NULL);
	}
		
};


void Joystick_Test(void *params){
	joystickValue value;
	int buttonValue;
	while(1){
		value = JoyStickValueCal();
		printf("xValue = %d, yValue = %d, ButtonValue = %d\n", value.xValue, value.yValue, g_ButtonState);
	}
}
