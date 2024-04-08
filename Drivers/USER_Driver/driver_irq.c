#include "driver_timer.h"
#include "stm32f4xx_hal.h"
#include "gpio.h"

extern void JoystickButton_IRQ_Callback(void);

/**********************************************************************
 * 函数名称： HAL_GPIO_EXTI_Callback
 * 功能描述： 外部中断的中断回调函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期：      版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/04	     V1.0	  韦东山	      创建
 ***********************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin)
    {
        case Button_Pin:
        {
            JoystickButton_IRQ_Callback();
            break;
        }
        default:
        {
            break;
        }
    }
}

