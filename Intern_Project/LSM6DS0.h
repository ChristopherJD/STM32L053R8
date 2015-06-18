#include "stm32l053xx.h"

#ifndef LSM6DS0_H
#define LSM6DS0_H

extern uint8_t LSM6DS0_Init(void);
extern void LSM6DS0_Configuration(void);
extern float LSM6DS0_X_Acceleration_Read(void);
extern float LSM6DS0_Y_Acceleration_Read(void);
extern float LSM6DS0_Z_Acceleration_Read(void);

#endif
