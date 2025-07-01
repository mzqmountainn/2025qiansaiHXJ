#include "sht20.h"   

void SHT20_Init(void)
{  
    IIC3.IIC_Init(&IIC3);
}

float ReadSht20(unsigned char whatdo)
{
	float temp;
  unsigned char MSB,LSB;
  float Humidity,Temperature;

	IIC3.IIC_Start(&IIC3);
	IIC3.IIC_SendByte(&IIC3,0x80);
	if(IIC3.IIC_WaitAck(&IIC3)==0)
	{
		IIC3.IIC_SendByte(&IIC3,whatdo);
		if(IIC3.IIC_WaitAck(&IIC3)==0)
		{
			do
			{
				rt_thread_delay(8);
				IIC3.IIC_Start(&IIC3);	
				IIC3.IIC_SendByte(&IIC3,0x81);
			}while(IIC3.IIC_WaitAck(&IIC3)==1);

			MSB = IIC3.IIC_ReadByte(&IIC3,1);
			LSB = IIC3.IIC_ReadByte(&IIC3,1);
			IIC3.IIC_ReadByte(&IIC3,0);
			IIC3.IIC_Stop(&IIC3);

			LSB &= 0xfc;
    	temp = MSB*256 + LSB;

			if (whatdo==((unsigned char)0xf5))
    		{        
        		Humidity =(temp*125)/65536-6;

        		return Humidity;
    		}                                                                                                  
    		else      
    		{        
        		Temperature = (temp*175.72)/65536-46.85;

        		return Temperature; 
    		}
		}
	}

	return 0;
}

float Get_Humidity(void)		//读取湿度
{
	float Humidity;
	Humidity = ReadSht20(0xf5);	       
	return Humidity;
	
}

float Get_Temperature(void)		//读取温度
{
	float Temperature;
	Temperature = ReadSht20(0xf3);
	return Temperature;
}



