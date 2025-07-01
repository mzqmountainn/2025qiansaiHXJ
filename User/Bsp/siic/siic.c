#include "siic.h"

/*---------------------------IIC函数定义-------------------------------------*/

static void iic_delay(uint32_t timeout)
{  
    uint16_t i;
    for(i=0;i<timeout;i++);
}

static uint8_t SDA_READ(const struct IIC_Type* IIC_Type_t)
{
	uint8_t rec;
	rec=GPIO_ReadInputDataBit(IIC_Type_t->GPIOx_SDA, IIC_Type_t->GPIO_PIN_SDA);
	return rec;
}

static void IIC_SCL(const struct IIC_Type* IIC_Type_t,uint8_t sat)
{
    if(sat==1)
    {
        GPIO_SetBits(IIC_Type_t->GPIOx_SCL, IIC_Type_t->GPIO_PIN_SCL);
    }
    else
    {
        GPIO_ResetBits(IIC_Type_t->GPIOx_SCL, IIC_Type_t->GPIO_PIN_SCL);
    }
}

static void IIC_SDA(const struct IIC_Type* IIC_Type_t,uint8_t sat)
{
    if(sat==1)
    {
        GPIO_SetBits(IIC_Type_t->GPIOx_SDA, IIC_Type_t->GPIO_PIN_SDA);
    }
    else
    {
        GPIO_ResetBits(IIC_Type_t->GPIOx_SDA, IIC_Type_t->GPIO_PIN_SDA);
    }
};

static void IIC_Init_t(const struct IIC_Type* IIC_Type_t)
{
     GPIO_InitTypeDef GPIO_InitStructure;
     
     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	/* The GPIOB clock was enabled  */
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);	/* The GPIOE clock was enabled  */
     
     GPIO_InitStructure.GPIO_Pin = IIC_Type_t->GPIO_PIN_SCL;
	   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;  	
	   GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  	/* Push-pull mode */
	   GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	   GPIO_Init(IIC_Type_t->GPIOx_SCL, &GPIO_InitStructure); 
     GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;  	/* Push-pull mode */
     GPIO_InitStructure.GPIO_Pin = IIC_Type_t->GPIO_PIN_SDA;
     GPIO_Init(IIC_Type_t->GPIOx_SDA, &GPIO_InitStructure); 

     IIC_SCL(IIC_Type_t,1);
     IIC_SDA(IIC_Type_t,1);

}

static void IIC_Start_t(const struct IIC_Type* IIC_Type_t)
{   

    IIC_SCL(IIC_Type_t,1);
    IIC_SDA(IIC_Type_t,1);
    iic_delay(20);
    IIC_SDA(IIC_Type_t,0);
    iic_delay(20);
    IIC_SCL(IIC_Type_t,0);
    iic_delay(20);

}


static void IIC_Stop_t(const struct IIC_Type* IIC_Type_t)
{
    IIC_SDA(IIC_Type_t,0);
    IIC_SCL(IIC_Type_t,1);
    iic_delay(20);
    IIC_SDA(IIC_Type_t,1);
    iic_delay(20);
}

static uint8_t IIC_WaitAck_t(const struct IIC_Type* IIC_Type_t)
{
    uint8_t rec;
    IIC_SDA(IIC_Type_t,1);
    iic_delay(20);
    IIC_SCL(IIC_Type_t,1);
    iic_delay(20);
    if(1==SDA_READ(IIC_Type_t))
    {
        rec=1;
    }
    else
    {
        rec=0;
    }
     IIC_SCL(IIC_Type_t,0);
     iic_delay(20);
     return rec;
}

static void IIC_Ack_t(const struct IIC_Type* IIC_Type_t)
{
	IIC_SDA(IIC_Type_t,0);
    iic_delay(20);
    IIC_SCL(IIC_Type_t,1);
    iic_delay(20);
    IIC_SCL(IIC_Type_t,0);
    iic_delay(20);
    IIC_SDA(IIC_Type_t,1);
    
}

static void IIC_NAck_t(const struct IIC_Type* IIC_Type_t)
{
	IIC_SDA(IIC_Type_t,1);
    iic_delay(20);
    IIC_SCL(IIC_Type_t,1);
    iic_delay(20);
    IIC_SCL(IIC_Type_t,0);
    iic_delay(20);
}


static void IIC_SendByte_t(const struct IIC_Type* IIC_Type_t,uint8_t rxaddr)
{
    uint8_t i;
    IIC_SCL(IIC_Type_t,0);
    for(i=0;i<8;i++)
    {
        if(rxaddr&0x80)
        {
            IIC_SDA(IIC_Type_t,1);
        }
        else
        {
            IIC_SDA(IIC_Type_t,0);
        }

        iic_delay(20);
        IIC_SCL(IIC_Type_t,1);
        iic_delay(20);
        IIC_SCL(IIC_Type_t,0);

        if(i==7)
        {
            IIC_SDA(IIC_Type_t,1);
        }

        rxaddr<<=1;
        iic_delay(20);  
    }

}

static uint8_t IIC_ReadByte_t(const struct  IIC_Type* IIC_Type_t,uint8_t ack)
{
    uint8_t i;
    uint8_t value=0;

    for(i=0;i<8;i++)
    {
        value<<=1;
        IIC_SCL(IIC_Type_t,1);
        iic_delay(20);
        if(SDA_READ(IIC_Type_t))
        {
            value++;
        }
        IIC_SCL(IIC_Type_t,0);
        iic_delay(20);
    }
    if(!ack)
    {
        IIC_NAck_t(IIC_Type_t);
    }
    else
    {
        IIC_Ack_t(IIC_Type_t);
    }

    return value;
}

//IIC_TypeDef IIC1={
//    .GPIOx_SCL=GPIOB,
//    .GPIOx_SDA=GPIOB,
//    .GPIO_PIN_SCL=GPIO_Pin_10,
//    .GPIO_PIN_SDA=GPIO_Pin_11,
//    .IIC_Init=IIC_Init_t,
//    .IIC_Start=IIC_Start_t,
//    .IIC_WaitAck=IIC_WaitAck_t,
//    .IIC_SendByte=IIC_SendByte_t,
//    .IIC_ReadByte=IIC_ReadByte_t,
//    .IIC_Ack=IIC_Ack_t,
//    .IIC_NAck=IIC_NAck_t,
//    .IIC_Stop=IIC_Stop_t
//};

//IIC_TypeDef IIC2={
//    .GPIOx_SCL=GPIOB,
//    .GPIOx_SDA=GPIOB,
//    .GPIO_PIN_SCL=GPIO_Pin_6,
//    .GPIO_PIN_SDA=GPIO_Pin_7,
//    .IIC_Init=IIC_Init_t,
//    .IIC_Start=IIC_Start_t,
//    .IIC_WaitAck=IIC_WaitAck_t,
//    .IIC_SendByte=IIC_SendByte_t,
//    .IIC_ReadByte=IIC_ReadByte_t,
//    .IIC_Ack=IIC_Ack_t,
//    .IIC_NAck=IIC_NAck_t,
//    .IIC_Stop=IIC_Stop_t
//};

//IIC_TypeDef IIC3={
//    .GPIOx_SCL=GPIOE,
//    .GPIOx_SDA=GPIOE,
//    .GPIO_PIN_SCL=GPIO_Pin_2,
//    .GPIO_PIN_SDA=GPIO_Pin_3,
//    .IIC_Init=IIC_Init_t,
//    .IIC_Start=IIC_Start_t,
//    .IIC_WaitAck=IIC_WaitAck_t,
//    .IIC_SendByte=IIC_SendByte_t,
//    .IIC_ReadByte=IIC_ReadByte_t,
//    .IIC_Ack=IIC_Ack_t,
//    .IIC_NAck=IIC_NAck_t,
//    .IIC_Stop=IIC_Stop_t
//};

IIC_TypeDef IIC1={				//SDP810
    .GPIOx_SCL=GPIOB,
    .GPIOx_SDA=GPIOB,
    .GPIO_PIN_SCL=GPIO_Pin_15,//10,11->15,14
    .GPIO_PIN_SDA=GPIO_Pin_14,
    .IIC_Init=IIC_Init_t,
    .IIC_Start=IIC_Start_t,
    .IIC_WaitAck=IIC_WaitAck_t,
    .IIC_SendByte=IIC_SendByte_t,
    .IIC_ReadByte=IIC_ReadByte_t,
    .IIC_Ack=IIC_Ack_t,
    .IIC_NAck=IIC_NAck_t,
    .IIC_Stop=IIC_Stop_t
};

IIC_TypeDef IIC2={				//XGZP6859D
    .GPIOx_SCL=GPIOE,
    .GPIOx_SDA=GPIOE,
    .GPIO_PIN_SCL=GPIO_Pin_2,//B6,7->E2,3
    .GPIO_PIN_SDA=GPIO_Pin_3,
    .IIC_Init=IIC_Init_t,
    .IIC_Start=IIC_Start_t,
    .IIC_WaitAck=IIC_WaitAck_t,
    .IIC_SendByte=IIC_SendByte_t,
    .IIC_ReadByte=IIC_ReadByte_t,
    .IIC_Ack=IIC_Ack_t,
    .IIC_NAck=IIC_NAck_t,
    .IIC_Stop=IIC_Stop_t
};

IIC_TypeDef IIC3={				//SHT20
    .GPIOx_SCL=GPIOB,
    .GPIOx_SDA=GPIOB,
    .GPIO_PIN_SCL=GPIO_Pin_13,
    .GPIO_PIN_SDA=GPIO_Pin_12,
    .IIC_Init=IIC_Init_t,
    .IIC_Start=IIC_Start_t,
    .IIC_WaitAck=IIC_WaitAck_t,
    .IIC_SendByte=IIC_SendByte_t,
    .IIC_ReadByte=IIC_ReadByte_t,
    .IIC_Ack=IIC_Ack_t,
    .IIC_NAck=IIC_NAck_t,
    .IIC_Stop=IIC_Stop_t
};



