/*----------------------------------------------------------------------------
 * Name:    hTS221.h
 * Purpose: Retrieve Temperature and Humidity data
 * Date: 		6/18/15
 * Author:	Christopher Jordan - Denny
 *----------------------------------------------------------------------------
 * Note(s): See C file for more information
 *----------------------------------------------------------------------------*/

#include "stm32l053xx.h"

#ifndef HTS221_H
#define HTS221_H

extern void HTS221_Configuration(void);
extern uint8_t HTS221_Init(void);
extern float HTS221_Temp_Read(void);
extern float HTS221_Humidity_Read(void);

#endif
