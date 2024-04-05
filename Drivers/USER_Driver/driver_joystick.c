#include "adc.h"
#include "driver_joystick.h"

static uint32_t ADC_Buffer[100];//用于储存ADC向DMA转运的数据，长度100

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
 * 函数名称： JoyStickInit
 * 功能描述： 摇杆值计算，对于每个轴，将Buffer中的数据累加50次取平均值
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： struct JoystickerValue
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/3/14	     V1.0	  Ervin	      创建
 ***********************************************************************/

struct JoystickValue JoyStickValueCal(){
	uint8_t i;
	uint32_t x, y;
	struct JoystickValue value;
	for(i = 0,x =0,y=0; i < 100;)
	{
		x += ADC_Buffer[i++];
		y += ADC_Buffer[i++];
	}
	x /= 50;
	y /= 50;
	value.xValue = x;
	value.yValue = y;
	return value;
}	

void Joystick_Test(void){
	struct JoystickValue value;
	while(1){
		value = JoyStickValueCal();
		printf("xValue = %d, yValue = %d\n", value.xValue, value.yValue);
	}
}
