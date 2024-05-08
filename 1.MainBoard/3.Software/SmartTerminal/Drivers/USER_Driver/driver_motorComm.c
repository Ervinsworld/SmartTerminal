#include "can.h"
#include "driver_motorComm.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"               // ARM.FreeRTOS::RTOS:Event Groups

static MotorInf g_currentMotorInf;
//extern QueueHandle_t MotorRawQueueHandle;

/**********************************************************************
 * 函数名称： MotorCommInit
 * 功能描述： 电机通信初始化
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/3/14	     V1.0	  Ervin	      创建
 ***********************************************************************/



void MotorCommInit(){
	MyCAN_Init();
}

//角度获取
float getMotorAngle(){
	return g_currentMotorInf.angle;
}

//速度获取
float getMotorSpeed(){
	return g_currentMotorInf.speed;
}

/**********************************************************************
 * 函数名称： SendMessage2Motor
 * 功能描述： 向Motor发送一个数据帧
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/3/14	     V1.0	  Ervin	      创建
 ***********************************************************************/

void SendMessage2Motor(float voltage, uint8_t motorID)
{
	CAN_TxHeaderTypeDef header;
	header.IDE = CAN_ID_STD;
	header.RTR = CAN_RTR_DATA;
	header.DLC = 8;
	
	uint8_t data[8] = {0};
	
	if(motorID <= 4) //ID=1~4的情况
	{
		header.StdId = 0x100;
		memcpy(&data[(motorID-1)*2], &(int16_t){voltage*1000}, 2);
	}
	else if(motorID > 4) //ID=5~8的情况
	{
		header.StdId = 0x200;
		memcpy(&data[(motorID-5)*2], &(int16_t){voltage*1000}, 2);
	}
	
	uint32_t mailbox;
	HAL_CAN_AddTxMessage(&hcan1, &header, data, &mailbox);
}

/**********************************************************************
 * 函数名称： HAL_CAN_RxFifo0MsgPendingCallback
 * 功能描述： CAN收到数据的中断回调(使用xQueueSendFromISR保护数据程序会无故卡死，这里只能牺牲一些实时性在中断中处理数据)
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2024/3/14	     V1.0	  Ervin	      创建
 ***********************************************************************/

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef header;
	uint8_t rxData[8];
	
	if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &header, rxData) != HAL_OK)
		return;

	//xQueueOverwriteFromISR(MotorRawQueueHandle, &rxData, NULL);
	g_currentMotorInf.angle = *(int32_t*)&rxData[0] / 1000.0f;
	g_currentMotorInf.speed = *(int16_t*)&rxData[4] / 10.0f;

}


void MotorComm_Test(void){
	SendMessage2Motor(2, 1);
	printf("angle is %f\n", getMotorAngle());
	printf("speed is %f\n", getMotorSpeed());
}
