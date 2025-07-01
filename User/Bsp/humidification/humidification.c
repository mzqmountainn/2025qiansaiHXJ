#include "humidification.h"

void temperature_PWM_Init(uint16_t arr,uint16_t psc){
	  GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    RCC_AHB1PeriphClockCmd(TM_PWM_GPIO_CLK,ENABLE);
    RCC_APB1PeriphClockCmd(TM_TIM_CLK, ENABLE);

    GPIO_PinAFConfig(TM_PWM_GPIO_PORT,TM_PWM_PINSOURCE,TM_PWM_AF);	//复用
		
	TIM_ETRConfig(TM_TIM,TIM_ExtTRGPSC_OFF,TIM_ExtTRGPolarity_NonInverted,0x00);

    GPIO_InitStructure.GPIO_Pin = TM_PWM_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
    GPIO_Init(TM_PWM_GPIO_PORT, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period=arr;
    TIM_TimeBaseStructure.TIM_Prescaler=psc;
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseInit(TM_TIM, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 2160;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TM_TIM, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TM_TIM, TIM_OCPreload_Enable);
		
//    TIM_Cmd(TM_TIM,ENABLE);
	TIM_Cmd(TM_TIM,DISABLE);
//	TIM_SetCompare1(TIM2, 10200);		//100，不加热
	
}

