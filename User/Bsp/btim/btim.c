#include "btim.h"

uint16_t time_cnt;

void btim_init(void)
{
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    // Enable the TIM clock
    RCC_APB1PeriphClockCmd(BTIM_CLK, ENABLE);

    //1ms加1次，1s产生中断
    TIM_TimeBaseInitStruct.TIM_Prescaler=8400-1;   
    TIM_TimeBaseInitStruct.TIM_Period=10000-1;
	
    TIM_TimeBaseInit(BTIM, &TIM_TimeBaseInitStruct);
		TIM_ClearFlag(BTIM, TIM_FLAG_Update);
    TIM_ITConfig(BTIM,TIM_IT_Update,ENABLE);

    TIM_Cmd(BTIM, DISABLE);
	
//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
    NVIC_InitTypeDef NVIC_InitStruct;
			
    NVIC_InitStruct.NVIC_IRQChannel = BTIM_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
		
}

void BiPAP_clear_keep_time(void)
{
	TIM_Cmd(BTIM, DISABLE);										
	TIM_ClearFlag(BTIM, TIM_FLAG_Update); 			  //清除中断标志位
	time_cnt=0;							  								    //time_cnt清零
	TIM6->CNT=0;																	//清除计数值	
	TIM_Cmd(BTIM, ENABLE);					  	          //使能定时器，开始计时
}







