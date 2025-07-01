#include "lcd_init.h"

static void delay_us(uint32_t _us)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;

    // 计算需要的时钟数 = 延迟微秒数 * 每微秒的时钟数
    ticks = _us * (SystemCoreClock / 1000000);

    // 获取当前的SysTick值
    told = SysTick->VAL;

    while (1)
    {
        // 重复刷新获取当前的SysTick值
        tnow = SysTick->VAL;

        if (tnow != told)
        {
            if (tnow < told)
                tcnt += told - tnow;
            else
                tcnt += SysTick->LOAD - tnow + told;

            told = tnow;

            // 如果达到了需要的时钟数，就退出循环
            if (tcnt >= ticks)
                break;
        }
    }
}

 void lcdInit(void)
 {
   GPIO_InitTypeDef  GPIO_InitStructure;
	
 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOG, ENABLE);//使能PORTA~E,PORTG时钟
 	
	 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_13|GPIO_Pin_15;    	//A5(SCL) A7(SDA) E8(RES) E10(DC) E12(CS) E14(BLK)
													//F14(SCL) E8(SDA) F13(RES) F15(DC) E7(CS) E9(BLK)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_9;    	//A5(SCL) A7(SDA) E8(RES) E10(DC) E12(CS) E14(BLK)  
													//F14(SCL) E8(SDA) F13(RES) F15(DC) E7(CS) E9(BLK)
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//GPIO_Mode_AF
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	
	GPIO_SetBits(GPIOF,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
	GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_7|GPIO_Pin_9);
  //    GPIO_InitTypeDef  GPIO_InitStructure;
	
 	// RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOG, ENABLE);//使能PORTA~E,PORTG时钟
 	
	 
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;    	//A5(SCL) A7(SDA) E8(RES) E10(DC) E12(CS) E14(BLK)
	// 												//F14(SCL) E8(SDA) F13(RES) F15(DC) E7(CS) E9(BLK)
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	
	// GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	// GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  //   GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_10|GPIO_Pin_12|GPIO_Pin_14;    	//A5(SCL) A7(SDA) E8(RES) E10(DC) E12(CS) E14(BLK)  
	// 												//F14(SCL) E8(SDA) F13(RES) F15(DC) E7(CS) E9(BLK)
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;	//GPIO_Mode_AF
	// GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	// GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  //   GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	
	// GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_7);
	// GPIO_SetBits(GPIOE,GPIO_Pin_8|GPIO_Pin_10|GPIO_Pin_12|GPIO_Pin_14);
  //   //初始化按键引脚
	// RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	// GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 |GPIO_Pin_10 |GPIO_Pin_11 |GPIO_Pin_12 |GPIO_Pin_13;
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	// GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	// GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	// GPIO_Init(GPIOC, &GPIO_InitStructure);
  // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	// GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	// GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	// GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	// GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	// GPIO_Init(GPIOC, &GPIO_InitStructure);
 }


/******************************************************************************
      函数说明：LCD串行数据写入函数
      入口数据：dat  要写入的串行数据
      返回值：  无
******************************************************************************/
void LCD_Writ_Bus(u8 dat) 
{	
	 u8 i;
	 LCD_CS_Clr();
	 for(i=0;i<8;i++)
	 {			  
		LCD_SCLK_Clr();
		if(dat&0x80)
		{
		   LCD_MOSI_Set();
		}
		else
		{
		   LCD_MOSI_Clr();
		}
		LCD_SCLK_Set();
		dat<<=1;
	 }	
	 LCD_CS_Set();	
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入数据
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	LCD_Writ_Bus(dat>>8);
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入命令
      入口数据：dat 写入的命令
      返回值：  无
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	LCD_DC_Clr();//写命令
	LCD_Writ_Bus(dat);
	LCD_DC_Set();//写数据
}


/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//储存器写
}
/******************************************************************************
      函数说明：LCD初始化函数
      入口数据：无
      返回值：  无
******************************************************************************/
void LCD_Init(void)
{
		lcdInit();//初始化GPIO
		LCD_RES_Set();
		delay_us(10);	
		LCD_RES_Clr();//复位
		delay_us(10);
		LCD_RES_Set();
		delay_us(120);
		LCD_BLK_Set();//打开背光

		LCD_WR_REG(0x11);     
		delay_us(120);                

		LCD_WR_REG(0x36);     
		if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
		else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
		else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
		else LCD_WR_DATA8(0xA0);

		LCD_WR_REG(0x3A);     
		LCD_WR_DATA8( 0x05);   //16BIT

		LCD_WR_REG(0xB2);     
		LCD_WR_DATA8( 0x05);   
		LCD_WR_DATA8( 0x05);   
		LCD_WR_DATA8( 0x00);   
		LCD_WR_DATA8( 0x33);   
		LCD_WR_DATA8( 0x33);   

		LCD_WR_REG(0xB7);     
		LCD_WR_DATA8( 0x35);   


		LCD_WR_REG(0xBB);     
		LCD_WR_DATA8( 0x21);   

		LCD_WR_REG(0xC0);     
		LCD_WR_DATA8( 0x2C);   

		LCD_WR_REG(0xC2);     
		LCD_WR_DATA8( 0x01);   

		LCD_WR_REG(0xC3);     
		LCD_WR_DATA8( 0x0B);   

		LCD_WR_REG(0xC4);     
		LCD_WR_DATA8( 0x20);   

		LCD_WR_REG(0xC6);     
		LCD_WR_DATA8( 0x0F);   //60HZ dot inversion

		LCD_WR_REG(0xD0);     
		LCD_WR_DATA8( 0xA7);   
		LCD_WR_DATA8( 0xA1); 

		LCD_WR_REG(0xD0);     
		LCD_WR_DATA8( 0xA4);   
		LCD_WR_DATA8( 0xA1);   
			

		LCD_WR_REG(0xD6);     
		LCD_WR_DATA8( 0xA1);   

		LCD_WR_REG(0xE0);     
		LCD_WR_DATA8( 0xD0);   
		LCD_WR_DATA8( 0x04);   
		LCD_WR_DATA8( 0x08);   
		LCD_WR_DATA8( 0x0A);   
		LCD_WR_DATA8( 0x09);   
		LCD_WR_DATA8( 0x05);   
		LCD_WR_DATA8( 0x2D);   
		LCD_WR_DATA8( 0x43);   
		LCD_WR_DATA8( 0x49);   
		LCD_WR_DATA8( 0x09);   
		LCD_WR_DATA8( 0x16);   
		LCD_WR_DATA8( 0x15);   
		LCD_WR_DATA8( 0x26);   
		LCD_WR_DATA8( 0x2B);   

		LCD_WR_REG(0xE1);     
		LCD_WR_DATA8( 0xD0);   
		LCD_WR_DATA8( 0x03);   
		LCD_WR_DATA8( 0x09);   
		LCD_WR_DATA8( 0x0A);   
		LCD_WR_DATA8( 0x0A);   
		LCD_WR_DATA8( 0x06);   
		LCD_WR_DATA8( 0x2E);   
		LCD_WR_DATA8( 0x44);   
		LCD_WR_DATA8( 0x40);   
		LCD_WR_DATA8( 0x3A);   
		LCD_WR_DATA8( 0x15);   
		LCD_WR_DATA8( 0x15);   
		LCD_WR_DATA8( 0x26);   
		LCD_WR_DATA8( 0x2A);   

		LCD_WR_REG(0x21);     

		LCD_WR_REG(0x29);     

		delay_us(10);                	
}








