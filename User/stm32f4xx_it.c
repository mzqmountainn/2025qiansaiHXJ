/* 该文件统一用于存放中断服务函数 */
#include "stm32f4xx.h"
#include "btim.h"
#include "led.h"
#include "rtthread.h"


void BTIM_IRQHandler(void)
{
    if(TIM_GetITStatus(BTIM,TIM_IT_Update)!=RESET) 
    {				
        time_cnt++;			
    }
  
    TIM_ClearITPendingBit(BTIM,TIM_IT_Update); 
}


