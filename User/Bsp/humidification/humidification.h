#ifndef __HUMIDIFICATION_H
#define __HUMIDIFICATION_H	 
#include "stm32f4xx.h"


/*************************PWM**********************/
#define TM_PWM_PIN             GPIO_Pin_5              //TIM2_CH1_ETR
#define TM_PWM_GPIO_PORT       GPIOA                      
#define TM_PWM_GPIO_CLK        RCC_AHB1Periph_GPIOA
#define TM_PWM_PINSOURCE		    GPIO_PinSource5
#define TM_PWM_AF			        GPIO_AF_TIM2

#define TM_TIM           			TIM2
#define TM_TIM_CLK       			RCC_APB1Periph_TIM2

#define TM_TIM_IRQn		    		TIM2_IRQn
#define TM_TIM_IRQHandler     TIM2_IRQHandler

void temperature_PWM_Init(uint16_t arr,uint16_t psc);

#endif
