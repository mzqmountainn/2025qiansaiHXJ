#include "led.h"   

void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_GPIOF,ENABLE); 
										   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_10;			
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;      		
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_Init(GPIOF, &GPIO_InitStructure);								   
	
	LED0=1;
	LED1=1;
}




/*********************************************END OF FILE**********************/

