#include "stm32f4xx.h"
#include "stm32f4xx_rtc.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_pwr.h"
#include "MyRTC.h"
#include "myflash.h"
extern char IsFirstTimeUse ;//来自main
void myRTC_Config(void);
void myRTC_TimeRegulate(void);
// void RTC_GetTime(void);

void myRTC_init(void)
{
    // 启用电源时钟和备份寄存器时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_BackupAccessCmd(ENABLE); // 允许访问备份寄存器
    
    // 检查LSE是否已经就绪
    RCC_LSEConfig(RCC_LSE_ON); // 启用LSE晶振
    while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET); // 等待LSE稳定

    // 配置RTC时钟源
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); // 使用LSE作为RTC时钟源
    RCC_RTCCLKCmd(ENABLE); // 启用RTC时钟

    RTC_WaitForSynchro(); // 等待RTC寄存器同步
    
    // 配置RTC
    if(IsFirstTimeUse)
    {myRTC_Config();}
    
    // 配置时间
    // myRTC_TimeRegulate();

    // while (1)
    // {
    //     // 获取并显示时间
    //     RTC_GetTime();
    // }
}

void myRTC_Config(void)
{
    RTC_InitTypeDef RTC_InitStructure;
    
    // 配置RTC预分频器，确保1秒基准
    RTC_InitStructure.RTC_AsynchPrediv = 127;
    RTC_InitStructure.RTC_SynchPrediv = 255;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStructure);
}

void myRTC_TimeRegulate(void)
{
    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_DateTypeDef RTC_DateStructure;

    //判断是否需要从flash读取时间
    if(IsFirstTimeUse){
    

    // 设置时间: 13:00:00
    RTC_TimeStructure.RTC_Hours = 13;
    RTC_TimeStructure.RTC_Minutes = 0;
    RTC_TimeStructure.RTC_Seconds = 0;
    
    // 设置日期: 2024-10-20
    RTC_DateStructure.RTC_Year = 24; // 2024年
    RTC_DateStructure.RTC_Month = RTC_Month_October;
    RTC_DateStructure.RTC_Date = 20;
    RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Sunday;


    //设置时间函数
    RTC_SetDate(RTC_Format_BIN, &RTC_DateStructure);
    RTC_SetTime(RTC_Format_BIN, &RTC_TimeStructure);
    }
}

// void RTC_GetTime(void)
// {
//     RTC_TimeTypeDef RTC_TimeStructure;
//     RTC_DateTypeDef RTC_DateStructure;

//     // 获取当前时间
//     RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
//     // 获取当前日期
//     RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

//     // 通过串口或其他方式输出时间和日期
//     printf("Time: %02d:%02d:%02d\n", RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
//     printf("Date: 20%02d-%02d-%02d\n", RTC_DateStructure.RTC_Year, RTC_DateStructure.RTC_Month, RTC_DateStructure.RTC_Date);
// }