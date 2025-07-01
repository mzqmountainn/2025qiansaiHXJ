#include "air_blower.h"

void TIMX_PWM_Init(uint16_t arr, uint16_t psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    RCC_AHB1PeriphClockCmd(PWM_GPIO_CLK,ENABLE);
    RCC_APB1PeriphClockCmd(GTIM_CLK, ENABLE);

    GPIO_PinAFConfig(PWM_GPIO_PORT,PWM_PINSOURCE,PWM_AF);

    GPIO_InitStructure.GPIO_Pin = PWM_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; 
    GPIO_Init(PWM_GPIO_PORT, &GPIO_InitStructure);

    TIM_TimeBaseStructure.TIM_Period=arr;
    TIM_TimeBaseStructure.TIM_Prescaler=psc;
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseInit(GTIM, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 2160;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(GTIM, &TIM_OCInitStructure);

    TIM_OC2PreloadConfig(GTIM, TIM_OCPreload_Enable);
    
    TIM_Cmd(GTIM,ENABLE);
}

void AIR_BLOWER_CONTROL_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(AIR_BLOWER_CONTROL_GPIO_CLK, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = AIR_BLOWER_CONTROL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(AIR_BLOWER_CONTROL_GPIO_PORT,&GPIO_InitStructure);

    AIR_BLOWER_PIN_LOW();
}






