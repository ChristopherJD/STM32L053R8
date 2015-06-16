#include "stm32l053xx.h"                  // Device header
#include <stdio.h>
#include "I2C.h"													// I2C Support
#include "Serial.h"
#include "LIS3MDL.h"

/*------------------------------------Slave Address--------------------------------------*/
#define LIS3MDL_ADDRESS			0x1E		//Slave Address without the r/w
/*------------------------------------LIS3MDL Registers----------------------------------*/
#define LIS3MDL_WHO_AM_I		0x0F		//Who am I register, contains device ID
#define LIS3MDL_OUT_X_L			0x28		//X coordinates Lower 8 bits
#define LIS3MDL_OUT_X_H			0x29		//X coordinates Upper 8 bits
#define LIS3MDL_OUT_Y_L			0x2A		//Y coordinates Lower 8 bits
#define LIS3MDL_OUT_Y_H			0x2B		//Y coordinates Upper 8 bits
#define LIS3MDL_OUT_Z_L			0x2C		//Z coordinates Lower 8 bits
#define LIS3MDL_OUT_Z_H			0x2D		//Z coordinates Upper 8 bits
#define LIS3MDL_STATUS_REG	0x27		//Status register contains XYZ data available and overrun
#define LTS3MDL_CTRL_REG3		0x22		//Contains operating mode (similar to PD)
#define LTS3MDL_CTRL_REG4		0x23		//Determine Z power consumption
#define LTS3MDL_CTRL_REG5		0x24		//Contains BDU
/*------------------------------------Device ID -----------------------------------------*/
#define LIS3MDL_DEVICE_ID						0x3D		//Device ID in Who am I register						
/*------------------------------------Configuration Bits---------------------------------*/
#define LIS3MDL_STATUS_REG_ZYXDA	0x8		//XYZ data available
#define LIS3MDL_STATUS_REG_XDA		0x1		//X data ready
#define LIS3MDL_STATUS_REG_YDA		0x2		//Y data ready
#define LIS3MDL_STATUS_REG_ZDA		0x4		//Z data ready

uint8_t LIS3MDL_Init(void){
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
	
	
	
	return(Device_Found);
}
