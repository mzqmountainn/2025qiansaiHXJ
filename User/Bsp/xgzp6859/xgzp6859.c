#include "xgzp6859.h"

static void XGZP6859_WriteReg(uint8_t reg, uint8_t value)
{
    IIC2.IIC_Start(&IIC2);
    IIC2.IIC_SendByte(&IIC2,(XGZP6859_ADDR<<1)|0x00);
    IIC2.IIC_WaitAck(&IIC2);
    IIC2.IIC_SendByte(&IIC2,reg);
    IIC2.IIC_WaitAck(&IIC2);
    IIC2.IIC_SendByte(&IIC2,value);
    IIC2.IIC_Stop(&IIC2);

    rt_thread_delay(8);
}

static uint8_t XGZP6859_ReadReg(uint8_t reg, uint8_t len, uint8_t *reg_data)
{
    uint8_t i;
	
    IIC2.IIC_Start(&IIC2);
    IIC2.IIC_SendByte(&IIC2,(XGZP6859_ADDR<<1)|0x00);
    IIC2.IIC_WaitAck(&IIC2);
    IIC2.IIC_SendByte(&IIC2,reg);
    IIC2.IIC_WaitAck(&IIC2);
	  rt_thread_delay(8);
    
    IIC2.IIC_Start(&IIC2);
    IIC2.IIC_SendByte(&IIC2,(XGZP6859_ADDR<<1)|0x01);
    IIC2.IIC_WaitAck(&IIC2);
    
    for(i=0;i<(len-1);i++)
    {
        *reg_data=IIC2.IIC_ReadByte(&IIC2,1);
        reg_data++;
    }
    *reg_data=IIC2.IIC_ReadByte(&IIC2,0);
    IIC2.IIC_Stop(&IIC2);
    return 0;
}

void XGZP6859_Init(void)
{  
    uint8_t result = 0;
    IIC2.IIC_Init(&IIC2);
}

void GZP6859D_ReadPressureModeData(int32_t *pPressure)
{
	  uint8_t result = 0;
		uint8_t pressArr[4] = {0};
		int32_t press = 0;

		XGZP6859_WriteReg(GZP6859D_CMD_ADDR, GZP6859D_COM);       //8ms
		XGZP6859_ReadReg(GZP6859D_CMD_ADDR,1,&result);			      //8ms

		// 获取压力数据AD值
		XGZP6859_ReadReg(XGZP6859_PRESSURE_REG_DATA_H,3,&pressArr[1]);  //8ms

		// 压力计算公式
		press = (pressArr[0] << 24) + (pressArr[1] << 16) + (pressArr[2] << 8) + pressArr[3];
		if(press > 8388607)
		{
				press = press - 16777216;
		}
		press = press / GZP6859D_K_VALUE;   // 单位为Pa
		*pPressure = press;
}


void GZP6859D_ReadCombinedModeData(int32_t *pTemperature, int32_t *pPressure)
{		
    uint8_t result = 0;
    uint8_t tempArr[4] = {0};
    int32_t temp = 0;
    uint8_t pressArr[4] = {0};
    int32_t press = 0;
    
	  XGZP6859_WriteReg(GZP6859D_CMD_ADDR, GZP6859D_COM); //8ms
    XGZP6859_ReadReg(GZP6859D_CMD_ADDR,1,&result);			//8ms
    
    // 获取温度数据AD值
    XGZP6859_ReadReg(XGZP6859_TEMPERATURE_REG_DATA_H,2,&tempArr[2]); //8ms
    XGZP6859_ReadReg(GZP6859D_CMD_ADDR,1,&tempArr[3]);  //8ms

    // 温度计算公式
    temp = (tempArr[0] << 24) + (tempArr[1] << 16) + (tempArr[2] << 8) + tempArr[3];
    if(temp > 32768)
    {
        temp = temp - 65536;
    }
    temp = temp / 256;                  // 单位为℃
    *pTemperature = temp;
    
    // 获取压力数据AD值
    XGZP6859_ReadReg(XGZP6859_PRESSURE_REG_DATA_H,3,&pressArr[1]);  //8ms
 
    // 压力计算公式
    press = (pressArr[0] << 24) + (pressArr[1] << 16) + (pressArr[2] << 8) + pressArr[3];
    if(press > 8388607)
    {
        press = press - 16777216;
    }
    press = press / GZP6859D_K_VALUE;   // 单位为Pa
    *pPressure = press;
}



