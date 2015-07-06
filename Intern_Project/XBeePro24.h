/*-------------------------------------------------------------------------------------------------------------------
 * Name:    XBeePro24.c
 * Purpose: Initialize USART, XBee, and send all data acquired
 * Date: 		6/18/15
 * Author:	Christopher Jordan - Denny
 *-------------------------------------------------------------------------------------------------------------------
 * Note(s): 
 *-------------------------------------------------------------------------------------------------------------------*/
 
 /*----------------------------------------------Include Statments---------------------------------------------------*/
 #include "stm32l053xx.h"					//Specific Device header
 
 #ifndef XBeePro24_H
 #define XBeePro24_H
 
 typedef struct AT_Data
 {
	 char MY[6];		/* Source Address unique to this device */
	 char ID[6];		/* Personal Area Network */
	 char DH[6];		/* Destination register high */
	 char DL[6];		/* Destination register low */
 } AT_Data;
 
 extern void LPUART_Init(void);
 extern void XBee_Init(void);
 extern void LPUART1_Send(char c[]);
 extern void Read_Xbee_Init(void);
 
 #endif

