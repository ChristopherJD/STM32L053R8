/*----------------------------------------------------------------------------
 * Name:    LSM6DS0.h
 * Purpose: Retrieve gyroscope and acceleration data
 * Date: 		6/18/15
 * Author:	Christopher Jordan - Denny
 *----------------------------------------------------------------------------
 * Note(s): See C file for more info
 *----------------------------------------------------------------------------*/

#include "stm32l053xx.h"

#ifndef LSM6DS0_H
#define LSM6DS0_H

extern uint8_t LSM6DS0_Init(void);
extern void LSM6DS0_Configuration(void);
extern float LSM6DS0_X_Acceleration_Read(void);
extern float LSM6DS0_Y_Acceleration_Read(void);
extern float LSM6DS0_Z_Acceleration_Read(void);
extern float LSM6DS0_Gyroscope_Roll_Read(void);
extern float LSM6DS0_Gyroscope_Pitch_Read(void);
extern float LSM6DS0_Gyroscope_Yaw_Read(void);

#endif
