#include "can.h"
#include "driver_motorComm.h"
#include "FreeRTOS.h"
#include "Motor.h"

extern QueueHandle_t rawSpeedHandle;
extern QueueHandle_t rawAngleHandle;

/**********************************************************************
 * �������ƣ� MotorCommInit
 * ���������� ���ͨ�ų�ʼ��
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/3/14	     V1.0	  Ervin	      ����
 ***********************************************************************/



void MotorCommInit(){
	MyCAN_Init();
}


/**********************************************************************
 * �������ƣ� SendMessage2Motor
 * ���������� ��Motor����һ������֡
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/3/14	     V1.0	  Ervin	      ����
 ***********************************************************************/

void SendMessage2Motor(float voltage, uint8_t motorId)
{
	CAN_TxHeaderTypeDef header;
	header.IDE = CAN_ID_STD;
	header.RTR = CAN_RTR_DATA;
	header.DLC = 8;
	
	uint8_t data[8] = {0};
	
	if(motorID <= 4) //ID=1~4�����
	{
		header.StdId = 0x100;
		memcpy(&data[(motorID-1)*2], &(int16_t){voltage*1000}, 2);
	}
	else if(motorID > 4) //ID=5~8�����
	{
		header.StdId = 0x200;
		memcpy(&data[(motorID-5)*2], &(int16_t){voltage*1000}, 2);
	}
	
	uint32_t mailbox;
	HAL_CAN_AddTxMessage(&hcan1, &header, data, &mailbox);
}

/**********************************************************************
 * �������ƣ� HAL_CAN_RxFifo0MsgPendingCallback
 * ���������� CAN�յ����ݵ��жϻص�(ʹ��xQueueSendFromISR�������ݳ�����޹ʿ���������ֻ������һЩʵʱ�����ж��д�������)
 * ��������� ��
 * ��������� ��
 * �� �� ֵ�� ��
 * �޸�����        �汾��     �޸���	      �޸�����
 * -----------------------------------------------
 * 2024/3/14	     V1.0	  Ervin	      ����
 ***********************************************************************/

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	CAN_RxHeaderTypeDef header;
	uint8_t rxData[8];
	
	if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &header, rxData) != HAL_OK)
		return;
	
	if((rawSpeedHandle==0 || rawSpeedHandle==0))
		return;
	else{
			//�Ƕ����ݷ���ǰ�ĸ��ֽ�, ת�����ݷ��ڵ�5-6�ֽ�
		float angle = *(int32_t*)&rxData[0];
		float speed = *(int16_t*)&rxData[4];
		xQueueOverwriteFromISR(rawAngleHandle, &angle, NULL);
		xQueueOverwriteFromISR(rawSpeedHandle, &speed, NULL);
		//printf("%f, %f\n", a, b);
	}

//	g_currentMotorInf.angle = *(int32_t*)&rxData[0] / 1000.0f;
//	g_currentMotorInf.speed = *(int16_t*)&rxData[4] / 10.0f;

}


void MotorComm_Test(void){
	SendMessage2Motor(2, 1);
	printf("angle is %f\n", getMotorAngle());
	printf("speed is %f\n", getMotorSpeed());
}
