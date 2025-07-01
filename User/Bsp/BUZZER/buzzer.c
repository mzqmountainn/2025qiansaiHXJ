#include "buzzer.h"   
//高电平开启蜂鸣器
void BUZZER_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOA,ENABLE); 
										   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      		
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		//复用推挽	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);								   
	
	BUZZER=0;
	
}



