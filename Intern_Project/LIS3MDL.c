/*------------------------------------------------------------------------------------------------------
 * Name:    LIS3MDL.c
 * Purpose: Retrieve magnetometer data
 * Date: 		6/18/15
 * Author:	Christopher Jordan - Denny
 *------------------------------------------------------------------------------------------------------
 * Note(s): Sensor communicates using I2C.Reads magnetrometer data
 *----------------------------------------------------------------------------------------------------*/

/*------------------------------------Include Statements----------------------------------------------*/
#include "stm32l053xx.h"                  // Specific device header
#include <stdio.h>												// Standard input output
#include "I2C.h"													// I2C Support
#include "Serial.h"												// USART Drivers
#include "LIS3MDL.h"
/*------------------------------------Addresses-------------------------------------------------------*/
#define LIS3MDL_ADDRESS						0x1E		//Slave Address without the r/w
#define LIS3MDL_DEVICE_ID					0x3D		//Device ID in Who am I register				
/*------------------------------------LIS3MDL Registers-----------------------------------------------*/
#define LIS3MDL_WHO_AM_I					0x0F		//Who am I register, contains device ID
#define LIS3MDL_OUT_X_L						0x28		//X coordinates Lower 8 bits
#define LIS3MDL_OUT_X_H						0x29		//X coordinates Upper 8 bits
#define LIS3MDL_OUT_Y_L						0x2A		//Y coordinates Lower 8 bits
#define LIS3MDL_OUT_Y_H						0x2B		//Y coordinates Upper 8 bits
#define LIS3MDL_OUT_Z_L						0x2C		//Z coordinates Lower 8 bits
#define LIS3MDL_OUT_Z_H						0x2D		//Z coordinates Upper 8 bits
#define LIS3MDL_STATUS_REG				0x27		//Status register contains XYZ data available and overrun
#define LIS3MDL_CTRL_REG1					0x20		//Determine XY power consumption vs performance
#define LIS3MDL_CTRL_REG2					0x21		//Contains Full scale configuration
#define LTS3MDL_CTRL_REG3					0x22		//Contains operating mode (similar to PD)
#define LTS3MDL_CTRL_REG4					0x23		//Determine Z power consumption vs performance
#define LTS3MDL_CTRL_REG5					0x24		//Contains BDU				
/*------------------------------------Configuration Bits----------------------------------------------*/
#define LIS3MDL_STATUS_REG_ZYXDA	0x8		//XYZ data available
#define LIS3MDL_STATUS_REG_XDA		0x1		//X data ready
#define LIS3MDL_STATUS_REG_YDA		0x2		//Y data ready
#define LIS3MDL_STATUS_REG_ZDA		0x4		//Z data ready
#define LIS3MDL_CTRL_REG1_OM0			0x20	//Operating mode XY
#define LIS3MDL_CTRL_REG1_OM1			0x40	//Operating mode XY
#define LIS3MDL_CTRL_REG4_OMZ0		0x4		//Operating mode Z
#define LIS3MDL_CTRL_REG4_OMZ1		0x8		//Operating mode Z
#define LIS3MDL_CTRL_REG5_BDU			0x40	//Block Data update until MSB and LSB read
#define LIS3MDL_CTRL_REG3_MD0			0x1		//Power options
#define LIS3MDL_CTRL_REG3_MD1			0x2		//Power options
#define LIS3MDL_CTRL_REG1_DO0			0x4		//Data output rate
#define LIS3MDL_CTRL_REG1_DO1			0x8		//Data output rate
#define LIS3MDL_CTRL_REG1_DO2			0x10	//Data outptu rate
#define LIS3MDL_CTRL_REG2_FS0			0x20	//Full scale setting
#define LIS3MDL_CTRL_REG2_FS1			0x40	//Full scale setting
/*-------------------------------------Functions------------------------------------------------------*/

/**
  \fn					uint8_t LIS3MDL_Init(void)
  \brief			Initialize LIS3MDL magnetometer
	\returns		uint8_t Device_Found: 1 - Device found, 0 - Device not found	
*/

uint8_t LIS3MDL_Init(void){
	
	//Local Variables
	uint8_t Device_Found = 0;
	
	//Read data from register and check signature	
	I2C_Read_Reg(LIS3MDL_ADDRESS,LIS3MDL_WHO_AM_I);
	
	//Check if device signature is correct
	if (I2C1->RXDR == LIS3MDL_DEVICE_ID){
		Device_Found = 1;
	}
	else{
		Device_Found = 0;
	}
	
	if(Device_Found){
		
		//Performance Vs. Power consumption XY (medium), and set data rate to 10Hz
		I2C_Write_Reg(LIS3MDL_ADDRESS,LIS3MDL_CTRL_REG1,(LIS3MDL_CTRL_REG1_OM0 | LIS3MDL_CTRL_REG1_DO2));
		
		//Full scale = +/- 4 gauss (Default value) - just in case
		I2C_Write_Reg(LIS3MDL_ADDRESS,LIS3MDL_CTRL_REG2,0x0);
		
		//Performance Vs. Power consumption Z (medium)
		I2C_Write_Reg(LIS3MDL_ADDRESS,LTS3MDL_CTRL_REG4,LIS3MDL_CTRL_REG4_OMZ0);
		
		//Enable BDU so you ensure MSB and LSB have been read
		I2C_Write_Reg(LIS3MDL_ADDRESS,LTS3MDL_CTRL_REG5,LIS3MDL_CTRL_REG5_BDU);
	}
	
	return(Device_Found);
}

/**
  \fn					void LIS3MDL_Configuration(void)
  \brief			prints important LIS3MDL configuration registers
*/

void LIS3MDL_Configuration(void){
	printf("----------------Configuration Settings-------------------\r\n");	
	//Single Conversion mode
	I2C_Read_Reg(LIS3MDL_ADDRESS,LTS3MDL_CTRL_REG3);
	printf("Device Mode: %x\r\n",I2C1->RXDR);
	
	//Performance Vs. Power consumption XY (medium)
	I2C_Read_Reg(LIS3MDL_ADDRESS,LIS3MDL_CTRL_REG1);
	printf("XY Performance: %x\r\n",I2C1->RXDR);
	
	//Performance Vs. Power consumption XY (medium)
	I2C_Read_Reg(LIS3MDL_ADDRESS,LTS3MDL_CTRL_REG4);
	printf("Z Performance: %x\r\n",I2C1->RXDR);
	
	//Enable BDU so you ensure MSB and LSB have been read
	I2C_Read_Reg(LIS3MDL_ADDRESS,LTS3MDL_CTRL_REG5);
	printf("BDU Enabled: %x\r\n",I2C1->RXDR);
	
	//Enable BDU so you ensure MSB and LSB have been read
	I2C_Read_Reg(LIS3MDL_ADDRESS,LIS3MDL_CTRL_REG2);
	printf("Full scale mode: %x\r\n",I2C1->RXDR);
	
	printf("---------------------------------------------------------\r\n");
}

/**
  \fn					float LIS3MDL_X_Read(void)
  \brief			Reads the magnetic field from the X axis
	\returns 		float OUT_X: the magnetic field in mG
*/

float LIS3MDL_X_Read(void){
	
	//Local variables
	uint8_t LIS3MDL_STATUS = 0;
	uint8_t OUT_X_L = 0;
	uint8_t OUT_X_H = 0;
	float OUT_X = 0;
	int16_t Raw_X = 0;
	
	//Set device to continuous conversion mode
	I2C_Write_Reg(LIS3MDL_ADDRESS,LTS3MDL_CTRL_REG3,LIS3MDL_CTRL_REG3_MD0);
	
	//Wait for X coordinate data to be ready
	do{
		I2C_Read_Reg(LIS3MDL_ADDRESS,LIS3MDL_STATUS_REG);
		LIS3MDL_STATUS = I2C1->RXDR;
	}while((LIS3MDL_STATUS & LIS3MDL_STATUS_REG_XDA) == 0);
	
	//Read X Axis magnetic field
	I2C_Read_Reg(LIS3MDL_ADDRESS,LIS3MDL_OUT_X_L);
	OUT_X_L = I2C1->RXDR;
	
	I2C_Read_Reg(LIS3MDL_ADDRESS,LIS3MDL_OUT_X_H);
	OUT_X_H = I2C1->RXDR;
	
	//Process X coordinates
	Raw_X = ((OUT_X_H << 8) | OUT_X_L);
	
	/*
	 *1/6842 = ~0.146 mG/LSB according to datasheet
	 */
	OUT_X = (float)Raw_X * 0.146f;		// when using +/- 4 gauss

	return(OUT_X);
}

/**
  \fn					float LIS3MDL_Y_Read(void)
  \brief			Reads the magnetic field from the Y axis
	\returns 		float OUT_Y: the magnetic field in mG
*/

float LIS3MDL_Y_Read(void){
	
	//Local Variables
	uint8_t LIS3MDL_STATUS = 0;
	uint8_t OUT_Y_L = 0;
	uint8_t OUT_Y_H = 0;
	float OUT_Y = 0;
	int16_t Raw_Y = 0;
	
	//Set device to continuous conversion mode
	I2C_Write_Reg(LIS3MDL_ADDRESS,LTS3MDL_CTRL_REG3,LIS3MDL_CTRL_REG3_MD0);
	
	//Wait for X coordinate data to be ready
	do{
		I2C_Read_Reg(LIS3MDL_ADDRESS,LIS3MDL_STATUS_REG);
		LIS3MDL_STATUS = I2C1->RXDR;
	}while((LIS3MDL_STATUS & LIS3MDL_STATUS_REG_YDA) == 0);
	
	//Read Y Axis magnetic field
	I2C_Read_Reg(LIS3MDL_ADDRESS,LIS3MDL_OUT_Y_L);
	OUT_Y_L = I2C1->RXDR;
	
	I2C_Read_Reg(LIS3MDL_ADDRESS,LIS3MDL_OUT_Y_H);
	OUT_Y_H = I2C1->RXDR;
	
	/*
	1/6842 = ~0.146 mG/LSB according to datasheet
	*/
	Raw_Y = ((OUT_Y_H << 8) | OUT_Y_L);
	OUT_Y = (float)Raw_Y * 0.146f;		// when using +/- 4 gauss
	return(OUT_Y);
}

/**
  \fn					float LIS3MDL_Z_Read(void)
  \brief			Reads the magnetic field from the Z axis
	\returns 		float OUT_Z: the magnetic field in mG
*/

float LIS3MDL_Z_Read(void){
	
	//Local Variables
	uint8_t LIS3MDL_STATUS = 0;
	uint8_t OUT_Z_L = 0;
	uint8_t OUT_Z_H = 0;
	float OUT_Z = 0;
	int16_t Raw_Z = 0;
	
	//Set device to continuous conversion mode
	I2C_Write_Reg(LIS3MDL_ADDRESS,LTS3MDL_CTRL_REG3,LIS3MDL_CTRL_REG3_MD0);
	
	//Wait for X coordinate data to be ready
	do{
		I2C_Read_Reg(LIS3MDL_ADDRESS,LIS3MDL_STATUS_REG);
		LIS3MDL_STATUS = I2C1->RXDR;
	}while((LIS3MDL_STATUS & LIS3MDL_STATUS_REG_ZDA) == 0);
	
	//Read Z Axis magnetic field
	I2C_Read_Reg(LIS3MDL_ADDRESS,LIS3MDL_OUT_Z_L);
	OUT_Z_L = I2C1->RXDR;
	
	I2C_Read_Reg(LIS3MDL_ADDRESS,LIS3MDL_OUT_Z_H);
	OUT_Z_H = I2C1->RXDR;
	
	Raw_Z = ((OUT_Z_H << 8) | OUT_Z_L);
	
	/*
	1/6842 = ~0.146 mG/LSB according to datasheet
	*/
	OUT_Z = (float)Raw_Z * 0.146f;		// when using +/- 4 gauss
	return(OUT_Z);
}
