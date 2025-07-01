#ifndef __LCD_INIT_H
#define __LCD_INIT_H

#include "stm32f4xx.h"
#include "board.h"

#define USE_HORIZONTAL 3  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏
#define Chip_Selection 1   //设置芯片初始化 0为ILI9341  1为ST7789

#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 240
#define LCD_H 320

#else
#define LCD_W 320
#define LCD_H 240
#endif



//-----------------LCD端口定义---------------- 

#define LCD_SCLK_Clr() GPIO_ResetBits(GPIOF,GPIO_Pin_14)//SCL=SCLK
#define LCD_SCLK_Set() GPIO_SetBits(GPIOF,GPIO_Pin_14)

#define LCD_MOSI_Clr() GPIO_ResetBits(GPIOE,GPIO_Pin_8)//SDA=MOSI
#define LCD_MOSI_Set() GPIO_SetBits(GPIOE,GPIO_Pin_8)

#define LCD_RES_Clr()  GPIO_ResetBits(GPIOF,GPIO_Pin_13)//RES
#define LCD_RES_Set()  GPIO_SetBits(GPIOF,GPIO_Pin_13)

#define LCD_DC_Clr()   GPIO_ResetBits(GPIOF,GPIO_Pin_15)//DC
#define LCD_DC_Set()   GPIO_SetBits(GPIOF,GPIO_Pin_15)
 		     
#define LCD_CS_Clr()   GPIO_ResetBits(GPIOE,GPIO_Pin_7)//CS
#define LCD_CS_Set()   GPIO_SetBits(GPIOE,GPIO_Pin_7)

#define LCD_BLK_Clr()  GPIO_ResetBits(GPIOE,GPIO_Pin_9)//BLK
#define LCD_BLK_Set()  GPIO_SetBits(GPIOE,GPIO_Pin_9)
// #define LCD_SCLK_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_5)//SCL=SCLK
// #define LCD_SCLK_Set() GPIO_SetBits(GPIOA,GPIO_Pin_5)

// #define LCD_MOSI_Clr() GPIO_ResetBits(GPIOA,GPIO_Pin_7)//SDA=MOSI
// #define LCD_MOSI_Set() GPIO_SetBits(GPIOA,GPIO_Pin_7)

// #define LCD_RES_Clr()  GPIO_ResetBits(GPIOE,GPIO_Pin_8)//RES
// #define LCD_RES_Set()  GPIO_SetBits(GPIOE,GPIO_Pin_8)

// #define LCD_DC_Clr()   GPIO_ResetBits(GPIOE,GPIO_Pin_10)//DC
// #define LCD_DC_Set()   GPIO_SetBits(GPIOE,GPIO_Pin_10)
 		     
// #define LCD_CS_Clr()   GPIO_ResetBits(GPIOE,GPIO_Pin_12)//CS
// #define LCD_CS_Set()   GPIO_SetBits(GPIOE,GPIO_Pin_12)

// #define LCD_BLK_Clr()  GPIO_ResetBits(GPIOE,GPIO_Pin_14)//BLK
// #define LCD_BLK_Set()  GPIO_SetBits(GPIOE,GPIO_Pin_14)


void lcdInit(void);//初始化GPIO
void LCD_Writ_Bus(u8 dat);//模拟SPI时序
void LCD_WR_DATA8(u8 dat);//写入一个字节
void LCD_WR_DATA(u16 dat);//写入两个字节
void LCD_WR_REG(u8 dat);//写入一个指令
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//设置坐标函数
void LCD_Init(void);//LCD初始化

#endif




