// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright (c) 2008-2023 100askTeam : Dongshan WEI <weidongshan@qq.com> 
 * Discourse:  https://forums.100ask.net
 */

 
/*  Copyright (C) 2008-2023 深圳百问网科技有限公司
 *  All rights reserved
 *
 *
 * 免责声明: 百问网编写的文档，仅供学员学习使用，可以转发或引用(请保留作者信息)，禁止用于商业用途！
 * 免责声明: 百问网编写的程序，可以用于商业用途，但百问网不承担任何后果！
 * 
 * 
 * 本程序遵循GPL V3协议，使用请遵循协议许可
 * 本程序所用的开发板：	DShanMCU-F103
 * 百问网嵌入式学习平台：https://www.100ask.net
 * 百问网技术交流社区：	https://forums.100ask.net
 * 百问网官方B站：				https://space.bilibili.com/275908810
 * 百问网官方淘宝：			https://100ask.taobao.com
 * 联系我们(E-mail)：	  weidongshan@qq.com
 *
 * 版权所有，盗版必究。
 *  
 * 修改历史     版本号           作者        修改内容
 *-----------------------------------------------------
 * 2023.08.04      v01         百问科技      创建文件
 *-----------------------------------------------------
 */


#include "stm32f4xx_hal.h"
#include "driver_mpu6050.h"
#include <stdio.h>
//#include "driver_lcd.h"
//#include "driver_timer.h"

//****************************************
// 定义MPU6050内部地址
//****************************************
#define	MPU6050_SMPLRT_DIV		0x19  // 陀螺仪采样率，典型值：0x07(125Hz)
#define	MPU6050_CONFIG			0x1A  // 低通滤波频率，典型值：0x06(5Hz)
#define	MPU6050_GYRO_CONFIG		0x1B  // 陀螺仪自检及测量范围，典型值：0x18(不自检，2000deg/s)
#define	MPU6050_ACCEL_CONFIG	0x1C  // 加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)

#define	MPU6050_ACCEL_XOUT_H	0x3B
#define	MPU6050_ACCEL_XOUT_L	0x3C
#define	MPU6050_ACCEL_YOUT_H	0x3D
#define	MPU6050_ACCEL_YOUT_L	0x3E
#define	MPU6050_ACCEL_ZOUT_H	0x3F
#define	MPU6050_ACCEL_ZOUT_L	0x40
#define	MPU6050_TEMP_OUT_H		0x41
#define	MPU6050_TEMP_OUT_L		0x42
#define	MPU6050_GYRO_XOUT_H		0x43
#define	MPU6050_GYRO_XOUT_L		0x44
#define	MPU6050_GYRO_YOUT_H		0x45
#define	MPU6050_GYRO_YOUT_L		0x46
#define	MPU6050_GYRO_ZOUT_H		0x47
#define	MPU6050_GYRO_ZOUT_L		0x48

#define	MPU6050_PWR_MGMT_1		0x6B //电源管理，典型值：0x00(正常启用)
#define	MPU6050_PWR_MGMT_2		0x6C
#define	MPU6050_WHO_AM_I		0x75 //IIC地址寄存器(默认数值0x68，只读)


#define MPU6050_I2C_ADDR     0xD0
#define MPU6050_TIMEOUT     500
extern I2C_HandleTypeDef hi2c1;
static I2C_HandleTypeDef *g_pHI2C_MPU6050 = &hi2c1;

/**********************************************************************
 * 函数名称： MPU6050_WriteRegister
 * 功能描述： 写MPU6050寄存器
 * 输入参数： reg-寄存器地址, data-要写入的数据
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int MPU6050_WriteRegister(uint8_t reg, uint8_t data)
{
    uint8_t tmpbuf[2];

    tmpbuf[0] = reg;
    tmpbuf[1] = data;
    
    return HAL_I2C_Master_Transmit(g_pHI2C_MPU6050, MPU6050_I2C_ADDR, tmpbuf, 2, MPU6050_TIMEOUT);
}

/**********************************************************************
 * 函数名称： MPU6050_ReadRegister
 * 功能描述： 读MPU6050寄存器
 * 输入参数： reg-寄存器地址
 * 输出参数： pdata-用来保存读出的数据
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  韦东山	      创建
 ***********************************************************************/
int MPU6050_ReadRegister(uint8_t reg, uint8_t *pdata)
{
	return HAL_I2C_Mem_Read(g_pHI2C_MPU6050, MPU6050_I2C_ADDR, reg, 1, pdata, 1, MPU6050_TIMEOUT);
}

/**********************************************************************
 * 函数名称： MPU6050_Init
 * 功能描述： MPU6050初始化函数,
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  韦东山	      创建
 ***********************************************************************/
int MPU6050_Init(void)
{
	MPU6050_WriteRegister(MPU6050_PWR_MGMT_1, 0x00);	//解除休眠状态
	MPU6050_WriteRegister(MPU6050_PWR_MGMT_2, 0x00);
	MPU6050_WriteRegister(MPU6050_SMPLRT_DIV, 0x09);
	MPU6050_WriteRegister(MPU6050_CONFIG, 0x06);
	MPU6050_WriteRegister(MPU6050_GYRO_CONFIG, 0x18);
	MPU6050_WriteRegister(MPU6050_ACCEL_CONFIG, 0x18);
    return 0;
}

/**********************************************************************
 * 函数名称： MPU6050_GetID
 * 功能描述： 读取MPU6050 ID
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： -1 - 失败, 其他值 - ID
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  韦东山	      创建
 ***********************************************************************/
int MPU6050_GetID(void)
{
    uint8_t id;
	if(0 == MPU6050_ReadRegister(MPU6050_WHO_AM_I, &id))
        return id;
    else
        return -1;
}



/**********************************************************************
 * 函数名称： MPU6050_ReadData
 * 功能描述： 读取MPU6050数据
 * 输入参数： 无
 * 输出参数： pAccX/pAccY/pAccZ         - 用来保存X/Y/Z轴的加速度
 *            pGyroX/pGyroY/pGyroZ - 用来保存X/Y/Z轴的角速度
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  韦东山	      创建
 ***********************************************************************/
int MPU6050_ReadData(int16_t *pAccX, int16_t *pAccY, int16_t *pAccZ, int16_t *pGyroX, int16_t *pGyroY, int16_t *pGyroZ)
{
	uint8_t datal, datah;
    int err = 0;
	
	err |= MPU6050_ReadRegister(MPU6050_ACCEL_XOUT_H, &datah);
	err |= MPU6050_ReadRegister(MPU6050_ACCEL_XOUT_L, &datal);
	if(pAccX)
        *pAccX = (datah << 8) | datal;
	
	err |= MPU6050_ReadRegister(MPU6050_ACCEL_YOUT_H, &datah);
	err |= MPU6050_ReadRegister(MPU6050_ACCEL_YOUT_L, &datal);
	if(pAccY)
        *pAccY = (datah << 8) | datal;
	
	err |= MPU6050_ReadRegister(MPU6050_ACCEL_ZOUT_H, &datah);
	err |= MPU6050_ReadRegister(MPU6050_ACCEL_ZOUT_L, &datal);
	if(pAccZ)
        *pAccZ = (datah << 8) | datal;


	err |= MPU6050_ReadRegister(MPU6050_GYRO_XOUT_H, &datah);
	err |= MPU6050_ReadRegister(MPU6050_GYRO_XOUT_L, &datal);
	if(pGyroX)
        *pGyroX = (datah << 8) | datal;

	
	err |= MPU6050_ReadRegister(MPU6050_GYRO_YOUT_H, &datah);
	err |= MPU6050_ReadRegister(MPU6050_GYRO_YOUT_L, &datal);
	if(pGyroY)
        *pGyroY = (datah << 8) | datal;
	
	err |= MPU6050_ReadRegister(MPU6050_GYRO_ZOUT_H, &datah);
	err |= MPU6050_ReadRegister(MPU6050_GYRO_ZOUT_L, &datal);
	if(pGyroZ)
        *pGyroZ = (datah << 8) | datal;

    return err;	
}


/**********************************************************************
 * 函数名称： MPU6050_Test
 * 功能描述： MPU6050测试程序
 * 输入参数： 无
 * 输出参数： 无
 *            无
 * 返 回 值： 0 - 成功, 其他值 - 失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2023/08/03	     V1.0	  韦东山	      创建
 ***********************************************************************/
//void MPU6050_Test(void)
//{
//    int id;
//    int16_t AccX, AccY, AccZ, GyroX, GyroY, GyroZ;

//    MPU6050_Init();
//    
//    id = MPU6050_GetID();
//    LCD_PrintString(0, 0, "MPU6050 ID:");
//    LCD_PrintHex(0, 2, id, 1);

//    HAL_Delay(1000);
//    LCD_Clear();

//    while (1)
//    {    
//		
//        MPU6050_ReadData(&AccX, &AccY, &AccZ, &GyroX, &GyroY, &GyroZ);

//        LCD_PrintString(0, 0, "X: ");
//        LCD_PrintSignedVal(3, 0,  AccX);    
//        LCD_PrintSignedVal(12, 0, GyroX);    

//        LCD_PrintString(0, 2, "Y: ");
//        LCD_PrintSignedVal(3, 2,  AccY);    
//        LCD_PrintSignedVal(12, 2, GyroY);    

//        LCD_PrintString(0, 4, "Z: ");
//        LCD_PrintSignedVal(3, 4,  AccZ);    
//        LCD_PrintSignedVal(12, 4, GyroZ);   

//		LCD_Clear();
//    }
//}

void MPU6050_Test(void)
{
    int id;
    int16_t AccX, AccY, AccZ, GyroX, GyroY, GyroZ;

    MPU6050_Init();
    
    id = MPU6050_GetID();
//	printf("MPU6050 ID:%d\n", id);
    HAL_Delay(1000);

    while (1)
    {    
		
        MPU6050_ReadData(&AccX, &AccY, &AccZ, &GyroX, &GyroY, &GyroZ);

		//printf("%d,%d,%d\n", AccX, AccY, AccZ);
		//printf("%d,%d,%d\n", AccX, AccY, AccZ);
		printf("%d,%d,%d\n", GyroX, GyroY, GyroZ);


    }
}
