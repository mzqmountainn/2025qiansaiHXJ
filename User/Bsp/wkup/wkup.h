#ifndef __WKUP_H__
#define __WKUP_H__

#include "board.h"

#define WK_UP PAin(0)			
	 
uint8_t Check_WKUP(void);  			
void WKUP_Init(void); 			
void Standby_mode(void);	


#endif /* __BOARD_H__ */
