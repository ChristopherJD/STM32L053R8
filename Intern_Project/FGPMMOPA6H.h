/*------------------------------------------------------------------------------------------------------
 * Name:    FGPMMOPA6H.h
 * Purpose: Initializes USART1 on PA9 and PA10 and communicates with the 
						adafruit GPS Shield.
 * Date: 		6/18/15
 * Author:	Christopher Jordan - Denny
 *------------------------------------------------------------------------------------------------------
 * Note(s): This is created to be used with the adafruit GPS module.
 * A jumper between rx to pin 2 when on soft serial mode must 
 * be present.
 *----------------------------------------------------------------------------------------------------*/

/*-------------------------------------------Include Statements---------------------------------------*/
#include "stm32l053xx.h"

#ifndef FGPMMOPA6H_H
#define FGPMMOPA6H_H

extern volatile uint8_t Transmission_In_Progress;
extern char 						GGA_Message[128];											/* Original GGA message */
extern char 						GSA_Message[128];											/* Original GSA message */
extern char 						GSV_Message[128];											/* Original GSV message */
extern char 						RMC_Message[128];											/* Original RMC message */
extern char 						VTG_Message[128];											/* Original VTG message */

/* Raw RMC data */
typedef struct RMC_Data
{
	int New_Data_Ready;						/* New Data Recieved */
	char Message_ID[7];						/* RMC Protocol header */
	char UTC_Time[11];						/* hhmmss.sss */
	char Status[2];								/* A = data valid, V = data NOT valid */
	char Latitude[10];						/* ddmm.mmmm */
	char N_S_Indicator[3];				/* N = North, S = South */
	char Longitude[11];						/* dddmm.mmmm */
	char E_W_Indicator[3];				/* E = East, W = West */
	char Speed_Over_Ground[5];		/* In Knots */
	char Course_Over_Ground[7];		/* Degrees */
	char Date[7];									/* ddmmyy */
	char Mode[5];									/* A = autonomous mode, D = Differential mode, E = Estimated mode */
}RMC_Data;

/* Raw GGA data */
typedef struct GGA_Data
{
	int New_Data_Ready;								/* New Data Recieved */
	char Message_ID[7];								/* GGA Protocol header */
	char UTC_Time[11];								/* hhmmss.sss */
	char Latitude[10];								/* ddmm.mmmm */
	char N_S_Indicator[3];						/* N = North, S = South */
	char Longitude[11];								/* dddmm.mmmm */
	char E_W_Indicator[3];						/* E = East, W = West */
	char Position_Indicator[2];				/* 0 = Fix not available, 1 = GPS Fix, 2 = Differential GPS fix */
	char Satellites_Used[3];					/* Range from 0 - 14 */
	char HDOP[5];											/* Horizontal Dilution of Precision */
	char MSL_Altitude[6];							/* Antenna Altitude above or below mean sea level */
	char Units_Altitude[2];						/* Units of antenna altitude */
	char Geoidal_Seperation[6];				/*  */
	char Units_Geoidal_Seperation[2];	/* Units for geoidal seperation */
	char Age_Of_Diff_Corr[5];					/* Null fields when DGPS is not used (seconds) */
	char Checksum[3];									/* XOR checksum value */
}GGA_Data;

/* This is the data after it has been parsed properly formated */
typedef struct GPS_Data
{
	int Valid_Data;
	char Altitude[5];
	char TRF_Time[15];
	char Date[9];
	char Latitude[15];
	char Longitude[15];
	float Ground_Speed;
	char Packaged[100];								/* Repackaged Data */
}GPS_Data;

/* Initialization methods */
extern void USART1_Init(void);
extern void FGPMMOPA6H_Init(void);

/* USART Methods */
extern int USART1_GetChar(void);
extern char USART1_PutChar(char character);
extern void USART1_Read(void);
extern void USART1_Send(char c[]);

/* RMC Data */
extern void FGPMMOPA6H_Parse_RMC_Data(void);
extern char* FGPMMOPA6H_Get_RMC_UTC_Time(void);
extern char* FGPMMOPA6H_Get_RMC_Latitude(void);
extern char* FGPMMOPA6H_Get_RMC_Longitude(void);
extern float FGPMMOPA6H_Get_RMC_Ground_Speed(void);
extern int FGPMMOPA6H_Get_RMC_Status(void);
extern char* FGPMMOPA6H_Get_RMC_Date(void);
extern void Print_RMC_Data(void);

/*GPS Data*/
extern void FGPMMOPA6H_Get_GPS_Data(void);
extern char* FGPMMOPA6H_Package_Data(void);

void Init_Structs(void);
#endif
