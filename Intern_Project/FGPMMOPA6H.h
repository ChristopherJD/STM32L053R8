#include "stm32l053xx.h"

#ifndef FGPMMOPA6H_H
#define FGPMMOPA6H_H

extern volatile uint8_t Transmission_In_Progress;
extern char 						GGA_Message[128];											/* Original GGA message */
extern char 						GSA_Message[128];											/* Original GSA message */
extern char 						GSV_Message[128];											/* Original GSV message */
extern char 						RMC_Message[128];											/* Original RMC message */
extern char 						VTG_Message[128];											/* Original VTG message */

extern void USART1_Init(void);
extern int USART1_GetChar(void);
extern char USART1_PutChar(char character);
extern void USART1_Read(void);
extern void USART1_Send(char c[]);
extern void FGPMMOPA6H_Parse_RMC_Data(void);
extern void FGPMMOPA6H_Get_RMC_TRF_Time(void);
extern char* FGPMMOPA6H_Get_Latitude(void);
extern char* FGPMMOPA6H_Get_Longitude(void);
extern float FGPMMOPA6H_Get_Ground_Speed(void);
extern void FGPMMOPA6H_Init(void);

#endif
