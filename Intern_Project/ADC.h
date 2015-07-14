/*------------------------------------------------------------------------------------------------------
 * Name:    ADC.h
 * Purpose: Initializes the ADC and provides ADC reading function
 * Date: 		7/14/15
 * Author:	Christopher Jordan - Denny
 *------------------------------------------------------------------------------------------------------
 * Note(s):
 *----------------------------------------------------------------------------------------------------*/

#ifndef ADC_H
#define ADC_H

extern void ADC_Init(void);
extern void Unitialize_ADC(void);
extern int ADC_Pin(int pin);
extern float Voltage_Conversion(int ADC_Reading);

#endif
