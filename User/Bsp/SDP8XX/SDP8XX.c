#include "SDP8XX.h"

/*
        //	int8_t P[8];
        //	float dif_pressure=0;
        //	float tempre_value=0;
        //	float Scale_Factors=0;

		SDP810.SDP8XX_ReadData(&SDP810,0X3615,P);
		
		dif_pressure=(float)(((P[0]*256)+P[1])/240.0);
		tempre_value=(float)(((P[3]*256)+P[4])/200.0);
		Scale_Factors=(float)((P[6]*256)+P[7]);

		rt_kprintf("dif_pressure_thread1:%f\r\n",dif_pressure);			

		rt_kprintf("SDP810:\r\n");
		rt_kprintf("dif_pressure:%f\r\n",dif_pressure);		
		*/
//		rt_kprintf("tempre_value:%f\r\n",tempre_value);				
//		rt_kprintf("Scale_Factors:%f \r\n",Scale_Factors);

/*---------------------------Defining the SDP8XX funcation-------------------------------------*/

static void SDP8XX_Init_t(const struct SDP8CXX_Type* SDP8XX_Type_t)
{
    IIC1.IIC_Init(&IIC1);
}

static void SDP8XX_Write_Cmd_t(const struct SDP8CXX_Type* SDP8XX_Type_t,uint16_t cmd)
{
    IIC1.IIC_Start(&IIC1);
    IIC1.IIC_SendByte(&IIC1,0X4A);
    IIC1.IIC_WaitAck(&IIC1);
    IIC1.IIC_SendByte(&IIC1,cmd>>8);
    IIC1.IIC_WaitAck(&IIC1);
    IIC1.IIC_SendByte(&IIC1,cmd);
    IIC1.IIC_Stop(&IIC1);
    rt_thread_delay(10);
}

static void SDP8XX_ReadData_t(const struct SDP8CXX_Type* SDP8XX_Type_t,uint16_t Cmd,int8_t *P)
{
    uint8_t i=0;
    uint8_t ack=0;

    IIC1.IIC_Start(&IIC1);
    IIC1.IIC_SendByte(&IIC1,0X4a);
    IIC1.IIC_WaitAck(&IIC1);
    IIC1.IIC_SendByte(&IIC1,Cmd>>8);
    IIC1.IIC_WaitAck(&IIC1);
    IIC1.IIC_SendByte(&IIC1,Cmd);
    IIC1.IIC_WaitAck(&IIC1);
    IIC1.IIC_Stop(&IIC1);

    rt_thread_delay(10);
    IIC1.IIC_Start(&IIC1);
    IIC1.IIC_SendByte(&IIC1,0X4b);
    IIC1.IIC_WaitAck(&IIC1);

    for(i=0;i<9;i++)
	{
        if(i<8)
        {
            ack=1;
        }
        else
        {
            ack=0;

        }
        P[i]=IIC1.IIC_ReadByte(&IIC1,ack);
    }
	
    IIC1.IIC_Stop(&IIC1);
    rt_thread_delay(10);  
		
}

/*---------------------------ISDP8XX -------------------------------------*/
SDP8XX_TypeDef SDP810={ 

    .SDP8XX_Init=SDP8XX_Init_t,
    //.SDP8XX_Read_OneByte=SDP8XX_Read_OneByte_t,
    .SDP8XX_Write_Cmd=SDP8XX_Write_Cmd_t,
    //.SDP8XX_Write_OneByte=SDP8XX_Write_OneByte_t,
    .SDP8XX_ReadData=SDP8XX_ReadData_t 
};


