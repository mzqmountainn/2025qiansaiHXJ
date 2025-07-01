#include "lcd_init.h"

static void delay_us(uint32_t _us)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;

    // ������Ҫ��ʱ���� = �ӳ�΢���� * ÿ΢���ʱ����
    ticks = _us * (SystemCoreClock / 1000000);

    // ��ȡ��ǰ��SysTickֵ
    told = SysTick->VAL;

    while (1)
    {
        // �ظ�ˢ�»�ȡ��ǰ��SysTickֵ
        tnow = SysTick->VAL;

        if (tnow != told)
        {
            if (tnow < told)
                tcnt += told - tnow;
            else
                tcnt += SysTick->LOAD - tnow + told;

            told = tnow;

            // ����ﵽ����Ҫ��ʱ���������˳�ѭ��
            if (tcnt >= ticks)
                break;
        }
    }
}

 void lcdInit(void)
 {
   GPIO_InitTypeDef  GPIO_InitStructure;
	
 	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��PORTA~E,PORTGʱ��
 	
	 
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
	
 	// RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE|RCC_AHB1Periph_GPIOG, ENABLE);//ʹ��PORTA~E,PORTGʱ��
 	
	 
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
  //   //��ʼ����������
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
      ����˵����LCD��������д�뺯��
      ������ݣ�dat  Ҫд��Ĵ�������
      ����ֵ��  ��
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
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	LCD_Writ_Bus(dat>>8);
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      ����˵����LCDд������
      ������ݣ�dat д�������
      ����ֵ��  ��
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	LCD_DC_Clr();//д����
	LCD_Writ_Bus(dat);
	LCD_DC_Set();//д����
}


/******************************************************************************
      ����˵����������ʼ�ͽ�����ַ
      ������ݣ�x1,x2 �����е���ʼ�ͽ�����ַ
                y1,y2 �����е���ʼ�ͽ�����ַ
      ����ֵ��  ��
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
		LCD_WR_REG(0x2a);//�е�ַ����
		LCD_WR_DATA(x1);
		LCD_WR_DATA(x2);
		LCD_WR_REG(0x2b);//�е�ַ����
		LCD_WR_DATA(y1);
		LCD_WR_DATA(y2);
		LCD_WR_REG(0x2c);//������д
}
/******************************************************************************
      ����˵����LCD��ʼ������
      ������ݣ���
      ����ֵ��  ��
******************************************************************************/
void LCD_Init(void)
{
		lcdInit();//��ʼ��GPIO
		LCD_RES_Set();
		delay_us(10);	
		LCD_RES_Clr();//��λ
		delay_us(10);
		LCD_RES_Set();
		delay_us(120);
		LCD_BLK_Set();//�򿪱���

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








