#ifndef __SHT20_H
#define	__SHT20_H

#include "board.h"

//#define  SHT20_SCL   PEout(11)
//#define  SHT20_SDA   PEout(12)


// GPIO Òý½Å¶¨Òå
//#define  SHT20_SCL_GPIO_CLK        RCC_APB2Periph_GPIOE
//#define  SHT20_SCL_GPIO_PORT       GPIOE
//#define  SHT20_SCL_GPIO_PIN        GPIO_Pin_11

//#define  SHT20_SDA_GPIO_SDA        RCC_APB2Periph_GPIOE
//#define  SHT20_SDA_GPIO_PORT       GPIOE
//#define  SHT20_SDA_GPIO_PIN        GPIO_Pin_12

//#define  SHT20_SDA_READ()          GPIO_ReadInputDataBit(SHT20_SDA_GPIO_PORT, SHT20_SDA_GPIO_PIN)

void SHT20_Init(void);
float ReadSht20(unsigned char whatdo);
float Get_Humidity(void);
float Get_Temperature(void);

#endif 
