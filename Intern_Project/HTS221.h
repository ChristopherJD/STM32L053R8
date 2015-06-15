#include "stm32l053xx.h"

#ifndef HTS221_H
#define HTS221_H

extern void HTS221_Configuration(void);
extern void HTS221_Init(void);
extern float HTS221_Temp_Read(void);
extern float HTS221_Humidity_Read(void);

#endif
