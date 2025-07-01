#ifndef _AIR_BLOWER_H
#define _AIR_BLOWER_H

#include "stm32f4xx.h"

/*************************PWM**********************/
#define PWM_PIN             GPIO_Pin_7         
#define PWM_GPIO_PORT       GPIOA                     
#define PWM_GPIO_CLK        RCC_AHB1Periph_GPIOC
#define PWM_PINSOURCE		    GPIO_PinSource7
#define PWM_AF			        GPIO_AF_TIM3

#define GTIM           		TIM3
#define GTIM_CLK       		RCC_APB1Periph_TIM3

#define GTIM_IRQn		    TIM3_IRQn
#define GTIM_IRQHandler     TIM3_IRQHandler

/*************************Control pin definition**********************/
#define AIR_BLOWER_CONTROL_PIN          GPIO_Pin_6             
#define AIR_BLOWER_CONTROL_GPIO_PORT    GPIOA                      
#define AIR_BLOWER_CONTROL_GPIO_CLK     RCC_AHB1Periph_GPIOA

#define AIR_BLOWER_PIN_LOW()       {AIR_BLOWER_CONTROL_GPIO_PORT->BSRRH=AIR_BLOWER_CONTROL_PIN;}
#define AIR_BLOWER_PIN_HIGH()      {AIR_BLOWER_CONTROL_GPIO_PORT->BSRRL=AIR_BLOWER_CONTROL_PIN;}



void TIMX_PWM_Init(uint16_t arr, uint16_t psc);
void AIR_BLOWER_CONTROL_GPIO_Init(void);

#endif

