/*------------------------------------------------------------------------------------------------------
 * Name:    FGPMMOPA6H.c
 * Purpose: Initializes USART1 on PA9 and PA10 and communicates with the 
						adafruit GPS Shield.
 * Date: 		6/18/15
 * Author:	Christopher Jordan - Denny
 *------------------------------------------------------------------------------------------------------
 * Note(s): This is created to be used with the adafruit GPS module.
 * A jumper between rx to pin 2 when on soft serial mode must 
 * be present.
 *----------------------------------------------------------------------------------------------------*/

/*---------------------------------Include Statements-------------------------------------------------*/
#include "stm32l053xx.h"			//Specific Device Header
#include <stdio.h>						//Standard input and output
#include <string.h>						//Various useful string functions
#include <stdlib.h>						//Various useful conversion functions
#include "FGPMMOPA6H.h"
#include "Serial.h"						//USART2 computer communication

/*---------------------------------Define Statments---------------------------------------------------*/
#define TRUE				0x1				//Truth value is 1
#define FALSE				0x0				//False value is 0

// different commands to set the update rate from once a second (1 Hz) to 10 times a second (10Hz)
// Note that these only control the rate at which the position is echoed, to actually speed up the
// position fix you must also send one of the position fix rate commands below too.
#define PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ		"$PMTK220,10000*2F\r\n" // Once every 10 seconds, 100 millihertz.
#define PMTK_SET_NMEA_UPDATE_200_MILLIHERTZ		"$PMTK220,5000*1B\r\n"  // Once every 5 seconds, 200 millihertz.
#define PMTK_SET_NMEA_UPDATE_1HZ  						"$PMTK220,1000*1F\r\n"	// Once every 1 second, 1 Hz
#define PMTK_SET_NMEA_UPDATE_5HZ  						"$PMTK220,200*2C\r\n"		// 5 Times every second, 5 Hz
#define PMTK_SET_NMEA_UPDATE_10HZ 						"$PMTK220,100*2F\r\n"		// 10 Times every second, 10 Hz
// Position fix update rate commands.
#define PMTK_API_SET_FIX_CTL_100_MILLIHERTZ		"$PMTK300,10000,0,0,0,0*2C\r\n" // Once every 10 seconds, 100 millihertz.
#define PMTK_API_SET_FIX_CTL_200_MILLIHERTZ		"$PMTK300,5000,0,0,0,0*18\r\n"  // Once every 5 seconds, 200 millihertz.
#define PMTK_API_SET_FIX_CTL_1HZ							"$PMTK300,1000,0,0,0,0*1C\r\n"
#define PMTK_API_SET_FIX_CTL_5HZ							"$PMTK300,200,0,0,0,0*2F\r\n"
// Can't fix position faster than 5 times a second!

#define PMTK_SET_BAUD_57600										"$PMTK251,57600*2C\r\n"
#define PMTK_SET_BAUD_9600										"$PMTK251,9600*17\r\n"

// turn on only the second sentence (GPRMC)
#define PMTK_SET_NMEA_OUTPUT_RMCONLY 					"$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n"
// turn on GPRMC and GGA
#define PMTK_SET_NMEA_OUTPUT_RMCGGA						"$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
// turn on ALL THE DATA
#define PMTK_SET_NMEA_OUTPUT_ALLDATA					"$PMTK314,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"
// turn off output
#define PMTK_SET_NMEA_OUTPUT_OFF							"$PMTK314,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n"

#define PCLK	32000000									// Peripheral Clock
#define BAUD	9600											// Baud rate

/*---------------------------------NMEA Output Sentences----------------------------------------------*/
static const char GGA_Tag[] = "$GPGGA";
static const char GSA_Tag[] = "$GPGSA";
static const char GSV_Tag[] = "$GPGSV";
static const char RMC_Tag[] = "$GPRMC";
static const char VTG_Tag[] = "$GPVTG";

/*---------------------------------Globals------------------------------------------------------------*/
volatile int 				CharIndex = 0;												/* Character index of the char array */
const int 					NMEA_LENGTH = 128;										/* The max length of one NMEA line */
char 								Rx_Data[NMEA_LENGTH] = "0";						/* Rx Sring */
volatile uint8_t 		Transmission_In_Progress = FALSE;			/* Are we in between a $ and \n */
char 								GGA_Message[128];											/* Original GGA message */
char 								GSA_Message[128];											/* Original GSA message */
char 								GSV_Message[128];											/* Original GSV message */
char 								RMC_Message[128];											/* Original RMC message */
char 								VTG_Message[128];											/* Original VTG message */

/*---------------------------------Structure Instantiate----------------------------------------------*/
RMC_Data RMC;
GPS_Data GPS;
GGA_Data GGA;

/*---------------------------------Functions----------------------------------------------------------*/

/**
  \fn          void USART1_IRQHandler(void)
  \brief       Global interrupt handler for USART1, handles Rx interrupt
*/

void USART1_IRQHandler(void){
	
	if(USART1->ISR & USART_ISR_RXNE){
		
		/* Reads and CLEARS RXNE Flag */
    Rx_Data[CharIndex] = USART1->RDR;
		
		/* Data Not Ready */
		RMC.New_Data_Ready = FALSE;
		GGA.New_Data_Ready = FALSE;
		
		/* If Rx_Data = $, then we are in transmission */
		if(Rx_Data[CharIndex] == '$'){
			Transmission_In_Progress = TRUE;
		}
		
		/* If we are transmitting then save to proper message once complete */
		if(Transmission_In_Progress == TRUE){
			if(Rx_Data[CharIndex] == '\n'){
				if(strncmp(GGA_Tag,Rx_Data,(sizeof(GGA_Tag)-1)) == 0){
					strcpy(GGA_Message,Rx_Data);
					GGA.New_Data_Ready = TRUE;
				}
				if(strncmp(GSA_Tag,Rx_Data,(sizeof(GSA_Tag)-1)) == 0){
					strcpy(GSA_Message,Rx_Data);
				}
				if(strncmp(GSV_Tag,Rx_Data,(sizeof(GSV_Tag)-1)) == 0){
					strcpy(GSV_Message,Rx_Data);
				}
				if(strncmp(RMC_Tag,Rx_Data,(sizeof(RMC_Tag)-1)) == 0){
					strcpy(RMC_Message,Rx_Data);
					RMC.New_Data_Ready = TRUE;
				}
				if(strncmp(VTG_Tag,Rx_Data,(sizeof(VTG_Tag)-1)) == 0){
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
	
	 /* Local variables */
  uint16_t USARTDIV = 0;
  uint16_t USART_FRACTION = 0;
  uint16_t USART_MANTISSA = 0;

	RCC->IOPENR   |=   RCC_IOPENR_GPIOAEN;			/* Enable GPIOA clock */
	RCC->APB2ENR  |=   RCC_APB2ENR_USART1EN;    /* Enable USART#1 clock */
	
	//interrupt init
	NVIC_EnableIRQ(USART1_IRQn);
	NVIC_SetPriority(USART1_IRQn,0);
	
	//Make PA8 an input with a pull up resistor
	GPIOA->MODER &= ~(( 3ul << 2* 8) | ( 3ul << 2* 8) ); /* Set to input */
	GPIOA->PUPDR &= ~(( 3ul << 2* 8) | ( 3ul << 2* 8) ); /* Set to 0 */
	GPIOA->PUPDR |=  (( 2ul << 2* 8) | ( 2ul << 2* 8) ); /* Set to pull down */
	
	//Configure PA9 to USART1_TX, PA10 to USART1_RX
  GPIOA->AFR[1] &= ~((15ul << 4* 1) | (15ul << 4* 2) );		/* Set to 0 */
  GPIOA->AFR[1] |=  (( 4ul << 4* 1) | ( 4ul << 4* 2) );		/* Set to alternate function 4 */
  GPIOA->MODER  &= ~(( 3ul << 2* 9) | ( 3ul << 2* 10) );		/* Set to 0 */
  GPIOA->MODER  |=  (( 2ul << 2* 9) | ( 2ul << 2* 10) );		/* Set to alternate function mode */
	
	  /* Check to see if oversampling by 8 or 16 to properly set baud rate*/
  if((USART1->CR1 & USART_CR1_OVER8) == 1){
  	USARTDIV = PCLK/BAUD;
  	USART_FRACTION = ((USARTDIV & 0x0F) >> 1) & (0xB);
  	USART_MANTISSA = ((USARTDIV & 0xFFF0) << 4);
  	USARTDIV = USART_MANTISSA | USART_FRACTION;
  	USART1->BRR = USARTDIV;															/* 9600 Baud with 32MHz peripheral clock 8bit oversampling */
  }
  else{
  	USART1->BRR = PCLK/BAUD;														/* 9600 Baud with 32MHz peripheral clock 16bit oversampling */	
  }

  USART1->CR3    = 0x0000;								/* no flow control */
  //USART1->CR2    |= USART_CR2_SWAP;				/* Swap Tx and Rx */
	
	/* 1 stop bit, 8 data bits */
  USART1->CR1    = ((USART_CR1_RE) |												/* enable RX  */
                     (USART_CR1_TE) |												/* enable TX  */
                     (USART_CR1_UE) |      									/* enable USART */
										 (USART_CR1_RXNEIE));										/* Enable Interrupt */
}

/**
  \fn					FGPMMOPA6H_Init(void)
  \brief			Initializes the GPS module
							Configuration:
								*	5s Position echo time
								*	5s Update time 
								*	Outputs both GGA and RMC message
*/

void FGPMMOPA6H_Init(int Refresh_Rate){
	
	/* Initialize Structures */
	//Init_Structs();
	if(Refresh_Rate == 1){
		USART1_Send(PMTK_API_SET_FIX_CTL_100_MILLIHERTZ);		/* 10s Position echo time   */
		USART1_Send(PMTK_SET_NMEA_UPDATE_100_MILLIHERTZ);		/* 10s update time 			   */
	}
	if(Refresh_Rate == 2){
		USART1_Send(PMTK_API_SET_FIX_CTL_200_MILLIHERTZ);		/* 5s Position echo time   */
		USART1_Send(PMTK_SET_NMEA_UPDATE_200_MILLIHERTZ);		/* 5s update time 			   */
	}
	if(Refresh_Rate == 3){
		USART1_Send(PMTK_API_SET_FIX_CTL_1HZ);							/* 1s Position echo time   */
		USART1_Send(PMTK_SET_NMEA_UPDATE_1HZ);							/* 1s update time 			   */
	}
	if(Refresh_Rate == 4){
		USART1_Send(PMTK_API_SET_FIX_CTL_5HZ);							/* 5 times every second Position echo time   */
		USART1_Send(PMTK_SET_NMEA_UPDATE_5HZ);							/* 5 times every second update time 			   */
	}
	
	USART1_Send(PMTK_SET_NMEA_OUTPUT_RMCGGA);					  /* Output RMC Data and GGA */
	printf("#####  GPS             Initialized  #####\r\n");
}

/**
	\fn				char USART1_PutChar(char ch)
	\brief		Puts a character to the USART
	\returns	ch: The character written to the USART
*/

char USART1_PutChar(char ch) {

	//Wait for buffer to be empty
  while ((USART1->ISR & USART_ISR_TXE) == 0){
			//Nop
	}
	
	//Send character
  USART1->TDR = (ch);

  return (ch);
}

/**
	\fn				void USART1_Send(char c[])
	\brief		Sends a string to the USART
*/

void USART1_Send(char c[]){
	
	int String_Length = strlen(c);
	int Counter = 0;
	
	while(Counter < String_Length){
		USART1_PutChar(c[Counter]);
		Counter++;
	}
}

/**
	\fn				void FGPMMOPA6H_Parse_GGA(void)
	\brief		Parses the GGA data based on the , delimeter
*/

void FGPMMOPA6H_Parse_GGA(void){
	
	//Local Variables
	char GGA_Message_Copy[128] = "";
	const char delimeter[2] = ",";
	char *token = "";
	int i = 0;
	char temp[15][12];		/* [15][12]: 15 strings, of length 12 */
	
	/* Copy original GGA message */
	strcpy(GGA_Message_Copy,GGA_Message);
	
	//Seperate message
	/* get the first token */
	token = strtok(GGA_Message_Copy, delimeter);
	
	/* Walk through other tokens */
	while( token != NULL ) 
   {
		 strcpy(temp[i],token);
		 i++;
     token = strtok(NULL, delimeter);
   }
	 
	/* Copy the message into its individual components */
	strcpy(GGA.Message_ID,temp[0]);
	strcpy(GGA.UTC_Time,temp[1]);
	strcpy(GGA.Latitude,temp[2]);
	strcpy(GGA.N_S_Indicator,temp[3]);
	strcpy(GGA.Longitude,temp[4]);
	strcpy(GGA.E_W_Indicator,temp[5]);
	strcpy(GGA.Position_Indicator,temp[6]);
	strcpy(GGA.Satellites_Used,temp[7]);
	strcpy(GGA.HDOP,temp[8]);
	strcpy(GGA.MSL_Altitude,temp[9]);
	strcpy(GGA.Units_Altitude,temp[10]);
	strcpy(GGA.Geoidal_Seperation,temp[11]);
	strcpy(GGA.Units_Geoidal_Seperation,temp[12]);
	strcpy(GGA.Age_Of_Diff_Corr,temp[13]);
	strcpy(GGA.Checksum,temp[14]);
}

/**
	\fn				void Print_GGA_Data(void)
	\brief		Prints the GGA data after it has been parsed
*/

void Print_GGA_Data(void){
	printf("GGA_Message: %s\r\n",GGA.Message_ID);
	printf("UTC_Time: %s\r\n",GGA.UTC_Time);
	printf("Latitude: %s\r\n",GGA.Latitude);
	printf("N/S: %s\r\n",GGA.N_S_Indicator);
	printf("Longitude: %s\r\n",GGA.Longitude);
	printf("E/W: %s\r\n",GGA.E_W_Indicator);
	printf("Position: %s\r\n",GGA.Position_Indicator);
	printf("Satellites Used: %s\r\n",GGA.Satellites_Used);
	printf("HDOP: %s\r\n",GGA.HDOP);
	printf("MSL_Altitude: %s\r\n",GGA.MSL_Altitude);
	printf("Altitude Units: %s\r\n",GGA.Units_Altitude);
	printf("Geoidal_Seperation: %s\r\n",GGA.Geoidal_Seperation);
	printf("Geoidal Units: %s\r\n",GGA.Units_Geoidal_Seperation);
	printf("Age of different Corr: %s\r\n",GGA.Age_Of_Diff_Corr);
	printf("Checksum %s\r\n",GGA.Checksum);
}

/**
  \fn          void FGPMMOPA6H_Parse_RMC_Data(void)
  \brief       Parses the RMC Data based on the , delimeter					
*/

void FGPMMOPA6H_Parse_RMC_Data(void){
	
	//Local Variables
	char RMC_Message_Copy[128] = "";
	const char delimeter[2] = ",";
	char *token = "";
	int i = 0;
	char temp[11][12];			/* [11][12]: 11 strings, of length 12 */
	
	//Copy original RMC to a copy in order to not destroy message
	strcpy(RMC_Message_Copy,RMC_Message);
	
	//Seperated Message
	/* get the first token */
   token = strtok(RMC_Message_Copy, delimeter);
   
   /* walk through other tokens */
   while( token != NULL ) 
   {
		 strcpy(temp[i],token);
		 i++;
     token = strtok(NULL, delimeter);
   }
	 
	 //Copy the message into its individual components
	strcpy(RMC.Message_ID,temp[0]);
	strcpy(RMC.UTC_Time,temp[1]);
	strcpy(RMC.Status,temp[2]);
	strcpy(RMC.Latitude,temp[3]);
	strcpy(RMC.N_S_Indicator,temp[4]);
	strcpy(RMC.Longitude,temp[5]);
	strcpy(RMC.E_W_Indicator,temp[6]);
	strcpy(RMC.Speed_Over_Ground,temp[7]);
	strcpy(RMC.Course_Over_Ground,temp[8]);
	strcpy(RMC.Date,temp[9]);
	strcpy(RMC.Mode,temp[10]);
}

/**
	\fn				void Print_RMC_Data(void)
	\brief		Prints the RMC data after it has been parsed
*/

void Print_RMC_Data(void){
	printf("RMC_Message: %s\r\n",RMC.Message_ID);
	printf("UTC_Time: %s\r\n",RMC.UTC_Time);
	printf("Status: %s\r\n",RMC.Status);
	printf("Latitude: %s\r\n",RMC.Latitude);
	printf("N/S: %s\r\n",RMC.N_S_Indicator);
	printf("Longitude: %s\r\n",RMC.Longitude);
	printf("E/W: %s\r\n",RMC.E_W_Indicator);
	printf("Speed: %s\r\n",RMC.Speed_Over_Ground);
	printf("Course: %s\r\n",RMC.Course_Over_Ground);
	printf("Date: %s\r\n",RMC.Date);
	printf("Mode: %s\r\n",RMC.Mode);
}

/**
  \fn          void FGPMMOPA6H_Get_RMC_TRF_Time(void)
  \brief       Prints the Thief River falls time
*/

char* FGPMMOPA6H_Get_RMC_UTC_Time(void){
	
	/* Local Variables */
	char hh[3] = "";
	char mm[3] = "";
	char ss[3] = "";
	char sss[5] = "";
	int Hours = 0;
	
	/* Parse original UTC time */
	strncpy(hh,RMC.UTC_Time,2);
	strncpy(mm,(RMC.UTC_Time+2),2);
	strncpy(ss,(RMC.UTC_Time+4),2);
	strncpy(sss,(RMC.UTC_Time+7),3);		//We want to skip the decimal point
	
	/* Convert to integer and calculate TRF time */
	Hours = atoi(hh);
	Hours -= 5;
	
	/* Put into time format */
	sprintf(GPS.TRF_Time,"%i:%s:%s.%s",Hours,mm,ss,sss);
	
	return(GPS.TRF_Time);
}

/**
  \fn					char* FGPMMOPA6H_Get_Latitude(void)
  \brief			Retrieves the Latitude Coordinates
	\returns		GPS_Data.Latitude: The formatted Latitude data
*/

char* FGPMMOPA6H_Get_RMC_Latitude(void){
	
	/* Local Variables */
	char dd[3] = "";
	char mm[3] = "";
	char mmm[6] = "";
	
	/* Parse Original Latitude data */
	strncpy(dd,RMC.Latitude,2);
	strncpy(mm,(RMC.Latitude+2),2);
	strncpy(mmm,(RMC.Latitude+4),5);
	
	/* Recreate String with degree sign */
	sprintf(GPS.Latitude,"%s%c%s%s'%s",dd,248,mm,mmm,RMC.N_S_Indicator);
	
	return(GPS.Latitude);
}

/**
  \fn					char* FGPMMOPA6H_Get_Longitude(void)
  \brief			Retrieves the Longitude Coordinates
	\returns		GPS_Data.Longitude: The formatted Longitude data
*/

char* FGPMMOPA6H_Get_RMC_Longitude(void){
	
	/* Local Variables */
	char ddd[4] = "";
	char mm[3] = "";
	char mmm[6] = "";
	
	/* Parse Original Latitude data */
	strncpy(ddd,RMC.Longitude,3);
	strncpy(mm,(RMC.Longitude+3),2);
	strncpy(mmm,(RMC.Longitude+5),6);
	
	/* Recreate String with appropriate signs */
	sprintf(GPS.Longitude,"%s%c%s%s'%s",ddd,248,mm,mmm,RMC.E_W_Indicator);
	
	return(GPS.Longitude);
}

/**
  \fn					float FGPMMOPA6H_Get_Ground_Speed(void)
  \brief			Retrieves the speed over ground
	\returns		GPS_Data.Latitude: The formatted Latitude data
*/

float FGPMMOPA6H_Get_RMC_Ground_Speed(void){
	
	/* Local Variables */
	float MPH_Conversion = 1.15077945;	/* 1 Knot = 1.15..MPH */
	
	GPS.Ground_Speed = 0.0;
	
	/* Get ground speed */
	GPS.Ground_Speed = atof(RMC.Speed_Over_Ground);
	
	/* Convert Knots to MPH */
	GPS.Ground_Speed *= MPH_Conversion;
	
	return(GPS.Ground_Speed);
}

/**
  \fn					int FGPMMOPA6H_Get_RMC_Status(void)
  \brief			Retrieves RMC Status
	\returns		GPS_Data.Valid Data: Is the gps data valid
*/

int FGPMMOPA6H_Get_RMC_Status(void){
	
	if((strcmp(RMC.Status,"A")) == 0){
		GPS.Valid_Data = TRUE;
	}
	else GPS.Valid_Data = FALSE;
	
	return(GPS.Valid_Data);
}

/**
  \fn					char* FGPMMOPA6H_Get_RMC_Date(void)
  \brief			Retrieves RMC Date
	\returns		GPS_Data.Date: What is the date
*/

char* FGPMMOPA6H_Get_RMC_Date(void){
	char dd[3] = "";
	char mm[3] = "";
	char yy[3] = "";
	
	/* Parse Original Latitude data */
	strncpy(dd,RMC.Date,2);
	strncpy(mm,(RMC.Date+2),2);
	strncpy(yy,(RMC.Date+4),2);
	
	/* Recreate String in appropriate format */
	sprintf(GPS.Date,"%s/%s/%s",mm,dd,yy);
	
	return(GPS.Date);
}

/**
  \fn					char* FGPMMOPA6H_Get_GGA_Altitude(void)
  \brief			Retrieves altitude
	\returns		GPS_Data.Altitude: The formatted Altitude data from GGA
*/

char* FGPMMOPA6H_Get_GGA_Altitude(void){
	
	float Altitude_In_Ft = 0.0;
	
	Altitude_In_Ft = atof(GGA.MSL_Altitude) * 3.28084;
	
	sprintf(GPS.Altitude,"%f",Altitude_In_Ft);
	
	return(GPS.Altitude);
}

/**
  \fn					void FGPMMOPA6H_Get_GPS_Data(void)
  \brief			Retrieves RMC Date
	\returns		GPS_Data.Date: What is the date
*/

void FGPMMOPA6H_Get_GPS_Data(void){
	
	/* Wait for New data to come */
	while(RMC.New_Data_Ready == 0){
		//Nop
	}
	/* Parse the RMC and GCC data */
	FGPMMOPA6H_Parse_RMC_Data();
	FGPMMOPA6H_Parse_GGA();
		
	/* Print in standard format */
	printf("GPS Data is Valid: %i\r\n",FGPMMOPA6H_Get_RMC_Status());
	printf("Date: %s\r\n",FGPMMOPA6H_Get_RMC_Date());
	printf("Time: %s\r\n",FGPMMOPA6H_Get_RMC_UTC_Time());
	printf("Latitude: %s\r\n",FGPMMOPA6H_Get_RMC_Latitude());
	printf("Longitude: %s\r\n",FGPMMOPA6H_Get_RMC_Longitude());
	printf("Speed: %f MPH\r\n",FGPMMOPA6H_Get_RMC_Ground_Speed());
	printf("Altitude: %s ft \r\n",FGPMMOPA6H_Get_GGA_Altitude());
		
	/* Data has been read, set new data ready to 0 */
	RMC.New_Data_Ready = 0;
	GGA.New_Data_Ready = 0;
}

/**
  \fn					char* FGPMMOPA6H_Package_Data(void)
  \brief			Packages GPS Data
	\returns		GPS_Data.Packaged: All the data chosen to format
*/

char* FGPMMOPA6H_Package_Data(void){
	
	/* Wait for New data to come */
	while(RMC.New_Data_Ready == 0){
		//Nop
	}
	
	/* Parse the RMC and GCC data */
	FGPMMOPA6H_Parse_RMC_Data();
	FGPMMOPA6H_Parse_GGA();
	
	if(FGPMMOPA6H_Get_RMC_Status() == 1){
		
		/* Package the data */
		sprintf(
			GPS.Packaged,"$%i,%s,%s,%s,%f,%s\r\n",					/* GPS.Packaged is destination */
			FGPMMOPA6H_Get_RMC_Status(),								/* Valid data									 */
			FGPMMOPA6H_Get_RMC_UTC_Time(),							/* TRF Time										 */
			FGPMMOPA6H_Get_RMC_Latitude(),							/* Latitude										 */
			FGPMMOPA6H_Get_RMC_Longitude(),							/* Longitude									 */
			FGPMMOPA6H_Get_RMC_Ground_Speed(),					/* Speed											 */
			GGA.MSL_Altitude														/* Altitude										 */
		);
	}
	else{
		sprintf(GPS.Packaged,"");
	}
	
	/* Data has been read, set new data ready to 0 */
	RMC.New_Data_Ready = 0;
	GGA.New_Data_Ready = 0;
	
	return(GPS.Packaged);
}

/**
  \fn					void Init_Structs(void)
  \brief			Clears all the structs
*/

//void Init_Structs(void){
//	
//	/* Clear RMC Memory */
//	memset(RMC.Course_Over_Ground,0,sizeof(RMC.Course_Over_Ground));
//	memset(RMC.Date,0,sizeof(RMC.Date));
//	memset(RMC.E_W_Indicator,0,sizeof(RMC.E_W_Indicator));
//	memset(RMC.Latitude,0,sizeof(RMC.Latitude));
//	memset(RMC.Longitude,0,sizeof(RMC.Longitude));
//	memset(RMC.Message_ID,0,sizeof(RMC.Message_ID));
//	memset(RMC.Mode,0,sizeof(RMC.Mode));
//	memset(RMC.N_S_Indicator,0,sizeof(RMC.N_S_Indicator));
//	memset(RMC.Speed_Over_Ground,0,sizeof(RMC.Speed_Over_Ground));
//	memset(RMC.Status,0,sizeof(RMC.Status));
//	memset(RMC.UTC_Time,0,sizeof(RMC.UTC_Time));
//	RMC.New_Data_Ready = FALSE;
//	
//	/* Clear GGA Data */
//	memset(GGA.Age_Of_Diff_Corr,0,sizeof(GGA.Age_Of_Diff_Corr));
//	memset(GGA.Checksum,0,sizeof(GGA.Checksum));
//	memset(GGA.E_W_Indicator,0,sizeof(GGA.E_W_Indicator));
//	memset(GGA.Geoidal_Seperation,0,sizeof(GGA.Geoidal_Seperation));
//	memset(GGA.HDOP,0,sizeof(GGA.HDOP));
//	memset(GGA.Latitude,0,sizeof(GGA.Latitude));
//	memset(GGA.Longitude,0,sizeof(GGA.Longitude));
//	memset(GGA.Message_ID,0,sizeof(GGA.Message_ID));
//	memset(GGA.MSL_Altitude,0,sizeof(GGA.MSL_Altitude));
//	memset(GGA.N_S_Indicator,0,sizeof(GGA.N_S_Indicator));
//	memset(GGA.Position_Indicator,0,sizeof(GGA.Position_Indicator));
//	memset(GGA.Satellites_Used,0,sizeof(GGA.Satellites_Used));
//	memset(GGA.Units_Altitude,0,sizeof(GGA.Units_Altitude));
//	memset(GGA.Units_Geoidal_Seperation,0,sizeof(GGA.Units_Geoidal_Seperation));
//	memset(GGA.UTC_Time,0,sizeof(GGA.UTC_Time));
//	GGA.New_Data_Ready = FALSE;
//	
//	/* Clear GPS Data */
//	memset(GPS.Altitude,0,sizeof(GPS.Altitude));
//	memset(GPS.Date,0,sizeof(GPS.Date));
//	memset(GPS.Latitude,0,sizeof(GPS.Latitude));
//	memset(GPS.Longitude,0,sizeof(GPS.Longitude));
//	memset(GPS.Packaged,0,sizeof(GPS.Packaged));
//	memset(GPS.TRF_Time,0,sizeof(GPS.TRF_Time));
//}
