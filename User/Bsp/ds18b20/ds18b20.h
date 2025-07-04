#ifndef __DS18B20_H
#define	__DS18B20_H
 
#include "stm32f4xx.h"
//#include "delay.h"
#include "rtthread.h"
 
#define HIGH  1
#define LOW   0
 
#define DS18B20_CLK     RCC_AHB1Periph_GPIOE
#define DS18B20_PIN     GPIO_Pin_5                  
#define DS18B20_PORT		GPIOE
 
//带参宏，可以像内联函数一样使用,输出高电平或低电平
#define DS18B20_DATA_OUT(a)	if (a)	\
					GPIO_SetBits(DS18B20_PORT,DS18B20_PIN);\
					else		\
					GPIO_ResetBits(DS18B20_PORT,DS18B20_PIN)
 //读取引脚的电平
#define  DS18B20_DATA_IN()	   GPIO_ReadInputDataBit(DS18B20_PORT,DS18B20_PIN)
 
u8 DS18B20_Init(void);
float DS18B20_Get_Temp(void);
#endif
					
					



					