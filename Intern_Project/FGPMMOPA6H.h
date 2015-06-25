#ifndef FGPMMOPA6H_H
#define FGPMMOPA6H_H

extern char line[120];

extern void USART1_Init(void);
extern int USART1_GetChar(void);
extern char USART1_PutChar(char character);
extern void USART1_Read(void);
extern void USART1_Send(char c[]);
extern void FGPMMOPA6H_RMC_Data(void);
extern void FGPMMOPA6H_Get_RMC_TRF_Time(void);
extern char* FGPMMOPA6H_Get_Latitude(void);
extern char* FGPMMOPA6H_Get_Longitude(void);
extern float FGPMMOPA6H_Get_Ground_Speed(void);

#endif
