#ifndef __DRIVER_OLED_H
#define __DRIVER_OLED_H     

#include "i2c.h"
 
 #define u8 uint8_t
 #define u32 uint32_t
//    一些使用例子
//      OLED_ShowStr(0, 0, "hello world", 2);//显示字符串
//    OLED_ShowStr(0, 2, "hello world", 1);//显示字符串
//    OLED_ShowCN_STR(0, 4 , 0 , 8);
//      sprintf(num_temp_buffer,"show num:%0.2f",num);
//      OLED_ShowStr(0, 6, num_temp_buffer, 2);//显示字符串
//      OLED_CLS();
//      OLED_DrawBMP(0,0,128,7,BMP2);

#define OLED0561_ADD    0x78  // OLED的I2C地址（禁止修改）
#define COM             0x00  // OLED 指令（禁止修改）
#define DAT             0x40  // OLED 数据（禁止修改）

void WriteCmd(unsigned char I2C_Command);//写命令
void WriteDat(unsigned char I2C_Data);//写数据
void OLED_Init(void);//初始化
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_Data);//全屏填充
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);//显示字符串
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);//显示汉字
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,const unsigned char BMP[]);//显示图片（取模：阴，列行，逆，输出索引，精简）

void OLED_ShowCN_STR(u8 x , u8 y , u8 begin , u8 num);  //自己写的输出中文字符串

void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size);
u32 oled_pow(u8 m,u8 n);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2);//size2(16|12)
void drawInitBMP(void);
#endif


