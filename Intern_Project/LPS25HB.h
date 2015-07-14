/*----------------------------------------------------------------------------
 * Name:    LPS25HB.h
 * Purpose: Retrieve Pressure data
 * Date: 		6/18/15
 * Author:	Christopher Jordan - Denny
 *----------------------------------------------------------------------------
 * Note(s): See C file for more info
 *----------------------------------------------------------------------------*/

/*---------------------------Include Statements-------------------------------*/
#include "stm32l053xx.h"

#ifndef LPS25HB_H
#define LPS25HB_H

extern uint8_t LPS25HB_Init(void);
extern void LPS25HB_Configuration(void);
extern float LPS25HB_Pressure_Read(void);

#endif
