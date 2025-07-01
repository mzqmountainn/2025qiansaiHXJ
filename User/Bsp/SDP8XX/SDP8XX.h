#ifndef _SDP810_H
#define _SDP810_H

#include "stm32f4xx.h"
#include "rtthread.h"
#include "siic.h"

//Define the SDP8XX class
typedef struct SDP8CXX_Type
{    
    void (*SDP8XX_Init)(const struct SDP8CXX_Type*);
    void (*SDP8XX_Write_Cmd)(const struct SDP8CXX_Type*,uint16_t);
    void (*SDP8XX_ReadData)(const struct SDP8CXX_Type*,uint16_t,int8_t*);
    
}SDP8XX_TypeDef;

extern SDP8XX_TypeDef SDP810;

#endif

