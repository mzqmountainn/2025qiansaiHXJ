#ifndef __XGZP6859_H__
#define __XGZP6859_H__

#include "siic.h"
#include "rtthread.h"

#define XGZP6859_ADDR 0x6D

#define XGZP6859_PRESSURE_REG_DATA_L 0x08
#define XGZP6859_PRESSURE_REG_DATA_M 0x07
#define XGZP6859_PRESSURE_REG_DATA_H 0x06
#define XGZP6859_TEMPERATURE_REG_DATA_L 0x0A
#define XGZP6859_TEMPERATURE_REG_DATA_H 0x09

#define GZP6859D_CMD_ADDR            0x30
#define GZP6859D_COM                 0X0A
#define GZP6859D_ONE_PRESS           0x09
#define GZP6859D_SYS_CONFIG_ADDR     0xA5
#define GZP6859D_P_CONFIG_ADDR       0xA6

//#define GZP6859D_K_VALUE          8192
//#define GZP6859D_K_VALUE          4096
//#define GZP6859D_K_VALUE          2048
//#define GZP6859D_K_VALUE          1024
// #define GZP6859D_K_VALUE         512
#define GZP6859D_K_VALUE          256
//#define GZP6859D_K_VALUE            128
// #define GZP6859D_K_VALUE         64

void XGZP6859_Init(void);
void GZP6859D_ReadPressureModeData(int32_t *pPressure);
void GZP6859D_ReadCombinedModeData(int32_t *pTemperature, int32_t *pPressure);

#endif

