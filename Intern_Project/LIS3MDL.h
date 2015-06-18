/*----------------------------------------------------------------------------
 * Name:    LIS3MDL.h
 * Purpose: Retrieve magnetometer data
 * Date: 		6/18/15
 * Author:	Christopher Jordan - Denny
 *----------------------------------------------------------------------------
 * Note(s): See C file for more information
 *----------------------------------------------------------------------------*/

#include "stm32l053xx.h"

#ifndef LIS3MDL_H
#define LIS3MDL_H

extern uint8_t LIS3MDL_Init(void);
extern void LIS3MDL_Configuration(void);
extern float LIS3MDL_X_Read(void);
extern float LIS3MDL_Y_Read(void);
extern float LIS3MDL_Z_Read(void);

#endif
