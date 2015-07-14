/*------------------------------------------------------------------------------------------------------
 * Name:    Serial.c
 * Purpose: Used to communicat with your computer using USART2
 * Date: 		7/14/15
 * Author:	Christopher Jordan - Denny
 *------------------------------------------------------------------------------------------------------
 * Note(s):	SER_PutChar are used to redefine printf function
 *----------------------------------------------------------------------------------------------------*/

#ifndef SERIAL_H
#define SERIAL_H

extern void SER_Initialize(void);
extern int  SER_GetChar   (void);
extern char  SER_PutChar   (char c);

#endif
