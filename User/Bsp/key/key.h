#ifndef __KEY_H
#define __KEY_H	 
#include "stm32f4xx.h"
#include "rtthread.h"


#define KEY0 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) //PE4

#define KEY0_PRES 	1	//KEY0按下



void KEY_Init(void);	//IO初始化
uint8_t KEY_Scan(uint8_t);  		//按键扫描函数	

#endif
