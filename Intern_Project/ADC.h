#ifndef ADC_H
#define ADC_H

extern void ADC_Init(void);
extern void Unitialize_ADC(void);
extern int ADC_Pin(int pin);
extern float Voltage_Conversion(int ADC_Reading);

#endif
