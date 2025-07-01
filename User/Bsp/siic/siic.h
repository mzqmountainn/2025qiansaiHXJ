#ifndef __SIIC_H__
#define __SIIC_H__

#include "stm32f4xx.h"

//定义IIC的类
typedef struct IIC_Type
{
    //属性
    GPIO_TypeDef *GPIOx_SCL; 
    GPIO_TypeDef *GPIOx_SDA; 
    uint32_t GPIO_PIN_SCL;       
    uint32_t GPIO_PIN_SDA;      

    //操作
    void(*IIC_Init)(const struct IIC_Type*);                   
    void(*IIC_Start)(const struct IIC_Type*);                  
    void(*IIC_Stop)(const struct IIC_Type*);                   
    uint8_t(*IIC_WaitAck)(const struct IIC_Type*);             
    void (*IIC_Ack)(const struct IIC_Type*);                    
    void (*IIC_NAck)(const struct IIC_Type*);                  
    void (*IIC_SendByte)(const struct IIC_Type*, uint8_t);     
    uint8_t (*IIC_ReadByte)(const struct IIC_Type*,uint8_t);                                 
    
}IIC_TypeDef;

extern IIC_TypeDef IIC1;
extern IIC_TypeDef IIC2;
extern IIC_TypeDef IIC3;	


#endif /* __BOARD_H__ */
