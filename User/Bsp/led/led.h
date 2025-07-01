#ifndef __LED_H
#define	__LED_H

#include "board.h"


#define LED0 PFout(9)	
#define LED1 PFout(10)

void LED_Init(void);

#endif /* __LED_H */
