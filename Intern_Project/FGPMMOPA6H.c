/*-------------------------------------------------------------------------------------------------------------------
 * Name:    FGPMMOPA6H.c
 * Purpose: Initializes USART1 on PA9 and PA10 and communicates with the 
						adafruit GPS Shield.
 * Date: 		6/18/15
 * Author:	Christopher Jordan - Denny
 *-------------------------------------------------------------------------------------------------------------------
 * Note(s): This is created to be used with the adafruit GPS module.
 * A jumper between rx to pin 2 when on soft serial mode must 
 * be present.
 *-------------------------------------------------------------------------------------------------------------------*/

/*---------------------------------Include Statements----------------------------------------------------------------*/
#include "stm32l053xx.h"			//Specific Device Header
#include <stdio.h>
#include "USART1.h"
#include "Serial.h"
#include <string.h>
#include <stdlib.h>
/*---------------------------------Define Statments------------------------------------------------------------------*/
#define USART1EN		0x4000		//USART1 Clock enable bit
#define IOPAEN			0x1				//Enable port A clock
#define TRUE				0x1				//Truth value is 1
#define FALSE				0x0				//False value is 0

/*---------------------------------Baud Rate Calculations------------------------------------------------------------*/
#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
#define __USART_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))

/*---------------------------------NMEA Output Sentences--------------------------------------------------------------*/
static const char GGA[] = "$GPGGA";
static const char GSA[] = "$GPGSA";
static const char GSV[] = "$GPGSV";
static const char RMC[] = "$GPRMC";
static const char VTG[] = "$GPVTG";

/*---------------------------------Globals----------------------------------------------------------------------------*/
volatile int 				CharIndex = 0;												/* Character index of the char array */
const int 					NMEA_LENGTH = 128;										/* The max length of one NMEA line */
char 								Rx_Data[NMEA_LENGTH] = "0";						/* Rx Sring */
volatile uint8_t 		Transmission_In_Progress = FALSE;			/* Are we in between a $ and \n */
char 								GGA_Message[128];											/* Original GGA message */
char 								GSA_Message[128];											/* Original GSA message */
char 								GSV_Message[128];											/* Original GSV message */
char 								RMC_Message[128];											/* Original RMC message */
char 								VTG_Message[128];											/* Original VTG message */

/*---------------------------------Structure Declarations-------------------------------------------------------------*/
/* This is the raw RMC Data */
struct RMC_Data
{
	char Message_ID[7];						/* RMC Protocol header */
	char UTC_Time[11];						/* hhmmss.sss */
	char Status[2];								/* A = data valid, V = data NOT valid */
	char Latitude[10];						/* ddmm.mmmm */
	char N_S_Indicator[2];				/* N = North, S = South */
	char Longitude[11];						/* dddmm.mmmm */
	char E_W_Indicator[2];				/* E = East, W = West */
	char Speed_Over_Ground[5];		/* In Knots */
	char Course_Over_Ground[7];		/* Degrees */
	char Date[7];									/* ddmmyy */
	char Mode[5];									/* A = autonomous mode, D = Differential mode, E = Estimated mode */
}RMC_Data;

/* This is the data after it has been parsed */
struct GPS_Data
{
	char Latitude[15];
	char Longitude[15];
	float Ground_Speed;
}GPS_Data;

/*---------------------------------Functions-------------------------------------------------------------------------*/

/**
  \fn          void USART1_IRQHandler(void)
  \brief       Global interrupt handler for USART1, handles Rx interrupt
*/

void USART1_IRQHandler(void){
	
	if(USART1->ISR & USART_ISR_RXNE){
		
		/* Reads and CLEARS RXNE Flag */
    Rx_Data[CharIndex] = USART1->RDR;
		
		/* If Rx_Data = $, then we are in transmission */
		if(Rx_Data[CharIndex] == '$'){
			Transmission_In_Progress = TRUE;
		}
		
		/* If we are transmitting then save to proper message once complete */
		if(Transmission_In_Progress == TRUE){
			if(Rx_Data[CharIndex] == '\n'){
				if(strncmp(GGA,Rx_Data,(sizeof(GGA)-1)) == 0){
					strcpy(GGA_Message,Rx_Data);
				}
				if(strncmp(GSA,Rx_Data,(sizeof(GSA)-1)) == 0){
					strcpy(GSA_Message,Rx_Data);
				}
				if(strncmp(GSV,Rx_Data,(sizeof(GSV)-1)) == 0){
					strcpy(GSV_Message,Rx_Data);
				}
				if(strncmp(RMC,Rx_Data,(sizeof(RMC)-1)) == 0){
					strcpy(RMC_Message,Rx_Data);
				}
				if(strncmp(VTG,Rx_Data,(sizeof(VTG)-1)) == 0){
					strcpy(VTG_Message,Rx_Data);
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

/**
  \fn          void USART1_Init(void)
  \brief       Initializes USART1 on PA9 and PA10
								
*/

void USART1_Init(void){
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

//void USART1_Read(void){
//	 
//}

void USART1_Send(char c[]){
	
	int String_Length = strlen(c);
	int Counter = 0;
	
	while(Counter < String_Length){
		USART1_PutChar(c[Counter]);
		Counter++;
	}
}

void FGPMMOPA6H_RMC_Data(void){
	
	//Local Variables
	char RMC_Message_Copy[128];
	const char delimeter[2] = ",";
	char *token;
	int i = 0;
	char temp[11][12];			/* [11][12]: 11 strings, of length 12 */
	
	//Copy original GSV to a copy in order to not destroy message
	strcpy(RMC_Message_Copy,RMC_Message);
	
	//Seperated Message
	/* get the first token */
   token = strtok(RMC_Message, delimeter);
   
   /* walk through other tokens */
   while( token != NULL ) 
   {
		 strcpy(temp[i],token);
		 i++;
     token = strtok(NULL, delimeter);
   }
	 
	strcpy(RMC_Data.Message_ID,temp[0]);
	strcpy(RMC_Data.UTC_Time,temp[1]);
	strcpy(RMC_Data.Status,temp[2]);
	strcpy(RMC_Data.Latitude,temp[3]);
	strcpy(RMC_Data.N_S_Indicator,temp[4]);
	strcpy(RMC_Data.Longitude,temp[5]);
	strcpy(RMC_Data.E_W_Indicator,temp[6]);
	strcpy(RMC_Data.Speed_Over_Ground,temp[7]);
	strcpy(RMC_Data.Course_Over_Ground,temp[8]);
	strcpy(RMC_Data.Date,temp[9]);
	strcpy(RMC_Data.Mode,temp[10]);
	
	printf("RMC_Message: %s\r\n",RMC_Data.Message_ID);
	printf("UTC_Time: %s\r\n",RMC_Data.UTC_Time);
	printf("Status: %s\r\n",RMC_Data.Status);
	printf("Latitude: %s\r\n",RMC_Data.Latitude);
	printf("N/S: %s\r\n",RMC_Data.N_S_Indicator);
	printf("Longitude: %s\r\n",RMC_Data.Longitude);
	printf("E/W: %s\r\n",RMC_Data.E_W_Indicator);
	printf("Speed: %s\r\n",RMC_Data.Speed_Over_Ground);
	printf("Course: %s\r\n",RMC_Data.Course_Over_Ground);
	printf("Date: %s\r\n",RMC_Data.Date);
	printf("Mode: %s\r\n",RMC_Data.Mode);
}

/**
  \fn          void FGPMMOPA6H_Get_RMC_TRF_Time(void)
  \brief       Prints the Thief River falls time
*/

void FGPMMOPA6H_Get_RMC_TRF_Time(void){
	
	/* Local Variables */
	char hh[3];
	char mm[3];
	char ss[3];
	int Hours = 0;
	int Minutes = 0;
	int Seconds = 0;
	
	/* Parse original UTC time */
	strncpy(hh,RMC_Data.UTC_Time,2);
	strncpy(mm,(RMC_Data.UTC_Time+2),2);
	strncpy(ss,(RMC_Data.UTC_Time+4),2);
	
	/* Convert Char[] into integer */
	Hours = atoi(hh);
	Minutes = atoi(mm);
	Seconds = atoi(ss);
	
	/* Convert to Thief River Falls time */
	Hours -= 5;
	
	/* Print the time */
	printf("TRF Time: %i:%i:%i\r\n",Hours,Minutes,Seconds);
}

/**
  \fn					char* FGPMMOPA6H_Get_Latitude(void)
  \brief			Retrieves the Latitude Coordinates
	\returns		GPS_Data.Latitude: The formatted Latitude data
*/

char* FGPMMOPA6H_Get_Latitude(void){
	
	/* Local Variables */
	char dd[3];
	char mm[3];
	char mmm[6];
	char Degree_Sign[2] = {248,'\0'};
	char Minute_Sign[2] = "'";
	
	/* Parse Original Latitude data */
	strncpy(dd,RMC_Data.Latitude,2);
	strncpy(mm,(RMC_Data.Latitude+2),2);
	strncpy(mmm,(RMC_Data.Latitude+4),5);
	
	/* Recreate String with degree sign */
	strcpy(GPS_Data.Latitude,dd);
	strcat(GPS_Data.Latitude,Degree_Sign);
	strcat(GPS_Data.Latitude,mm);
	strcat(GPS_Data.Latitude,mmm);
	strcat(GPS_Data.Latitude,Minute_Sign);
	strcat(GPS_Data.Latitude,RMC_Data.N_S_Indicator);
	
	return(GPS_Data.Latitude);
}

/**
  \fn					char* FGPMMOPA6H_Get_Longitude(void)
  \brief			Retrieves the Longitude Coordinates
	\returns		GPS_Data.Longitude: The formatted Longitude data
*/

char* FGPMMOPA6H_Get_Longitude(void){
	
	/* Local Variables */
	char ddd[4];
	char mm[3];
	char mmm[6];
	char Degree_Sign[2] = {248,'\0'};
	char Minute_Sign[2] = "'";
	
	/* Parse Original Latitude data */
	strncpy(ddd,RMC_Data.Longitude,3);
	strncpy(mm,(RMC_Data.Longitude+3),2);
	strncpy(mmm,(RMC_Data.Longitude+5),5);
	
	/* Recreate String with degree sign */
	strcpy(GPS_Data.Longitude,ddd);							/* Strange error occuring here */
	strcat(GPS_Data.Longitude,Degree_Sign);
	strcat(GPS_Data.Longitude,mm);
	strcat(GPS_Data.Longitude,mmm);
	strcat(GPS_Data.Longitude,Minute_Sign);
	strcat(GPS_Data.Longitude,RMC_Data.E_W_Indicator);
	
	return(GPS_Data.Longitude);
}

/**
  \fn					float FGPMMOPA6H_Get_Ground_Speed(void)
  \brief			Retrieves the speed over ground
	\returns		GPS_Data.Latitude: The formatted Latitude data
*/

float FGPMMOPA6H_Get_Ground_Speed(void){
	
	/* Local Variables */
	float MPH_Conversion = 1.15077945;
	
	GPS_Data.Ground_Speed = 0.0;
	
	/* Get ground speed */
	GPS_Data.Ground_Speed = atof(RMC_Data.Speed_Over_Ground);
	
	/* Convert Knots to MPH */
	GPS_Data.Ground_Speed *= MPH_Conversion;
	
	return(GPS_Data.Ground_Speed);
}
