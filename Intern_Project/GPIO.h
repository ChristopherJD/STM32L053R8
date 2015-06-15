#ifndef GPIO_H
#define GPIO_H

extern void Initialize_Port_A(int pin, unsigned long mode, unsigned long output_Type, unsigned long output_Speed, unsigned long pupd);
extern void Initialize_Port_C(int pin, unsigned long mode, unsigned long output_Type, unsigned long output_Speed, unsigned long pupd);
extern void Unitialize_Port_C(void);
extern void Unitialize_Port_A(void);
extern void LED_Init(void);
extern int Button_Get_State (void);
extern void Button_Initialize(void);
extern void LED_On(void);
extern void LED_Off(void);

#endif
