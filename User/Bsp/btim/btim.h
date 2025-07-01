#ifndef __BTIM_H
#define __BTIM_H	 

#include "stm32f4xx.h"

#define BTIM           		  TIM6
#define BTIM_CLK       	  	RCC_APB1Periph_TIM6

#define BTIM_IRQn		      	TIM6_DAC_IRQn
#define BTIM_IRQHandler     TIM6_DAC_IRQHandler

extern uint16_t time_cnt;

void btim_init(void);
void BiPAP_clear_keep_time(void);

#endif
