/*----------------------------------------------------------------------------
 * Name:    USART1.c
 * Purpose: Initializes USART1 on PA9 and PA10.
 * Date: 		6/18/15
 * Author:	Christopher Jordan - Denny
 *----------------------------------------------------------------------------
 * Note(s): This is created to be used with the adafruit GPS module.
 * A jumper between rx to pin 2 when on soft serial mode must 
 * be present.
 *----------------------------------------------------------------------------*/

/*---------------------------------Include Statements-------------------------*/
#include "stm32l053xx.h"			//Specific Device Header
#include <stdio.h>
#include "USART1.h"
#include "Serial.h"
#include <string.h>
/*---------------------------------Define Statments---------------------------*/
#define USART1EN		0x4000		//USART1 Clock enable bit
#define IOPAEN			0x1				//Enable port A clock
#define TRUE				0x1				//Truth value is 1
#define FALSE				0x0				//False value is 0

//Baud rate calculation
#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
#define __USART_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))

//GPS Sentences
static const char GGA[] = "$GPGGA";
static const char GSA[] = "$GPGSA";
static const char GSV[] = "$GPGSV";
static const char RMC[] = "$GPRMC";
static const char VTG[] = "$GPVTG";

//Globals
volatile int 				CharIndex = 0;												/* Character index of the char array */
const int 					NMEA_LENGTH = 1000;										/* The max length of one NMEA line */
char 								Rx_Data[NMEA_LENGTH] = "0";						/* Rx Sring */
volatile uint8_t 		Transmission_In_Progress = FALSE;			/* Are we in between a $ and \n */
char 								Message1[128];														/* Storage for GPS Data */
char 								Message2[128];														/* Storage for GPS Data */
char 								Message3[128];														/* Storage for GPS Data */
char 								Message4[128];														/* Storage for GPS Data */
char 								Message5[128];														/* Storage for GPS Data */
char 								Message6[128];														/* Storage for GPS Data */
char 								Message7[128];														/* Storage for GPS Data */
char 								Message8[128];														/* Storage for GPS Data */
uint8_t							Data_Pointer = 0;

void USART1_IRQHandler(void){
	
	if(USART1->ISR & USART_ISR_RXNE){
		
		/* Reads and CLEARS RXNE Flag */
    Rx_Data[CharIndex] = USART1->RDR;
		
		/* If Rx_Data = $, then we are in transmission */
		if(Rx_Data[CharIndex] == '$'){
			Transmission_In_Progress = TRUE;
		}
		
		/* If we are transmitting then save to Data0 once complete */
		if(Transmission_In_Progress == TRUE){
			if(Rx_Data[CharIndex] == '\n'){
				switch(Data_Pointer){
					case 0:
						strcpy(Message1,Rx_Data);
						Data_Pointer = 1;
						break;
					case 1:
						strcpy(Message2,Rx_Data);
						Data_Pointer = 2;
						break;
					case 2:
						strcpy(Message3,Rx_Data);
						Data_Pointer = 3;
						break;
					case 3:
						strcpy(Message4,Rx_Data);
						Data_Pointer = 4;
						break;
					case 4:
						strcpy(Message5,Rx_Data);
						Data_Pointer = 5;
						break;
					case 5:
						strcpy(Message6,Rx_Data);
						Data_Pointer = 6;
						break;
					case 6:
						strcpy(Message7,Rx_Data);
						Data_Pointer = 7;
						break;
					case 7:
						strcpy(Message8,Rx_Data);
						Data_Pointer = 0;
						break;
				}
				Transmission_In_Progress = FALSE;
				CharIndex = 0;
				memset(Rx_Data,0,sizeof(Rx_Data));
			}
			else{
				CharIndex++;
			}
		}
	}
}

void USART1_Init(){
	RCC->IOPENR   |=   IOPAEN;			/* Enable GPIOA clock */
	RCC->APB2ENR  |=   USART1EN;    /* Enable USART#1 clock */
	
	//interrupt init
	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_SetPriority(USART1_IRQn,0);
	
	//Make PA8 an input with a pull up resistor
	GPIOA->MODER &= ~(( 3ul << 2* 8) | ( 3ul << 2* 8) ); /* Set to input */
	GPIOA->PUPDR &= ~(( 3ul << 2* 8) | ( 3ul << 2* 8) ); /* Set to 0 */
	GPIOA->PUPDR |=  (( 2ul << 2* 8) | ( 2ul << 2* 8) ); /* Set to pull up */
	
	//Configure PA9 to USART1_TX, PA10 to USART1_RX
  GPIOA->AFR[1] &= ~((15ul << 4* 1) | (15ul << 4* 2) );		/* Set to 0 */
  GPIOA->AFR[1] |=  (( 4ul << 4* 1) | ( 4ul << 4* 2) );		/* Set to alternate function 4 */
  GPIOA->MODER  &= ~(( 3ul << 2* 9) | ( 3ul << 2* 10) );		/* Set to 0 */
  GPIOA->MODER  |=  (( 2ul << 2* 9) | ( 2ul << 2* 10) );		/* Set to alternate function mode */
	
	USART1->BRR  = __USART_BRR(32000000ul, 9600ul);  /* 9600 baud @ 32MHz   */
  USART1->CR3    = 0x0000;                 /* no flow control */
  USART1->CR2    |= USART_CR2_SWAP;        /* Swap Tx and Rx */
  USART1->CR1    = ((   1ul <<  2) |       /* enable RX  */
                    (   1ul <<  3) |       /* enable TX  */
                    (   0ul << 12) |       /* 8 data bits */
                    (   0ul << 28) |       /* 8 data bits */
                    (   1ul <<  0) |      /* enable USART */
										(USART_CR1_RXNEIE));	/* Enable Interrupt */
}

/*----------------------------------------------------------------------------
  Read character from Serial Port
 *----------------------------------------------------------------------------*/
//int USART1_GetChar(void) {

//  if (USART1->ISR & USART_ISR_RXNE){
//    return (USART1->RDR);
//	}
//	else return (-1);
//	
//}

char USART1_PutChar(char ch) {

	//Wait for buffer to be empty
  while ((USART1->ISR & USART_ISR_TXE) == 0){
			//Nop
	}
	
	//Send character
  USART1->TDR = (ch);

  return (ch);
}

void USART1_Read(void){
	
	//Keeps printing Line, must fix this
	printf("%s",Message1);
	printf("%s",Message2);
	printf("%s",Message3);
	printf("%s",Message4);
	printf("%s",Message5);
	printf("%s",Message6);
	printf("%s",Message7);
	printf("%s",Message8);
	
	
}

void USART1_Send(char c[]){
	
	int String_Length = strlen(c);
	int Counter = 0;
	
	while(Counter < String_Length){
		USART1_PutChar(c[Counter]);
		Counter++;
	}
}
