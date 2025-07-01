#ifndef __KEY_H
#define __KEY_H	 
#include "stm32f4xx.h"
#include "rtthread.h"


#define KEY0 		GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) //PE4

#define KEY0_PRES 	1	//KEY0����



void KEY_Init(void);	//IO��ʼ��
uint8_t KEY_Scan(uint8_t);  		//����ɨ�躯��	

#endif
