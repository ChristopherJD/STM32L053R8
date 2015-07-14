/*------------------------------------------------------------------------------------------------------
 * Name:    ISK01A1.h
 * Purpose: Initialize ISK01A1 expansion board and get data from sensors
 * Date: 		6/18/15
 * Author:	Christopher Jordan - Denny
 *------------------------------------------------------------------------------------------------------
 * Note(s): See C file for further discription
 *----------------------------------------------------------------------------------------------------*/

/*-----------------------------------------Include Statements-----------------------------------------*/
#include "stm32l053xx.h"

#ifndef ISK01A1_H
#define ISK01A1_H

typedef struct HTS221_Data
{
	float Temperature;
	float Humidity;	
}HTS221_Data;

typedef struct LPS25HB_Data
{
	float Pressure;
}LPS25HB_Data;

typedef struct LIS3MDL_Data
{
	float X_Magnetic_Field;
	float Y_Magnetic_Field;
	float Z_Magnetic_Field;
}LIS3MDL_Data;

typedef struct LSM6DS0_Data
{
	float X_Acceleration;
	float Y_Acceleration;
	float Z_Acceleration;
	float Pitch;
	float Yaw;
	float Roll;
}LSM6DS0_Data;

typedef struct ISK01A1_Data
{
	char Packaged_Data[100];
	float Altitude;
}ISK01A1_Data;

/* Used for finding the altitude */
typedef struct Pressure_Data
{
	float Initial;
	float Current;
}Pressure_Data;

extern void ISK01A1_Init(void);
extern void ISK01A1_Configuration(void);
extern float ISK01A1_Get_Temperature(void);
extern float ISK01A1_Get_Humidity(void);
extern float ISK01A1_Get_Pressure(void);
extern float ISK01A1_Get_Magnetic_X(void);
extern float ISK01A1_Get_Magnetic_Y(void);
extern float ISK01A1_Get_Magnetic_Z(void);
extern float ISK01A1_Get_Acceleration_X(void);
extern float ISK01A1_Get_Acceleration_Y(void);
extern float ISK01A1_Get_Acceleration_Z(void);
extern float ISK01A1_Get_Roll(void);
extern float ISK01A1_Get_Pitch(void);
extern float ISK01A1_Get_Yaw(void);
extern float ISK01A1_Get_Altitude(void);
extern float QuadCopter_Altitude(void);
extern char* ISK01A1_Package_Data(void);

#endif
