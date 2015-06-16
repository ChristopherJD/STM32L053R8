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
#define LIS3MDL_CTRL_REG1		0x20		//Determine XY power consumption vs performance
#define LTS3MDL_CTRL_REG3		0x22		//Contains operating mode (similar to PD)
#define LTS3MDL_CTRL_REG4		0x23		//Determine Z power consumption vs performance
#define LTS3MDL_CTRL_REG5		0x24		//Contains BDU
/*------------------------------------Device ID -----------------------------------------*/
#define LIS3MDL_DEVICE_ID						0x3D		//Device ID in Who am I register						
/*------------------------------------Configuration Bits---------------------------------*/
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
/*-------------------------------------Global Variabls-----------------------------------*/
uint8_t OUT_X_L = 0;
uint8_t OUT_X_H = 0;
uint8_t OUT_Y_L = 0;
uint8_t OUT_Y_H = 0;
uint8_t OUT_Z_L = 0;
uint8_t OUT_Z_H = 0;

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
	
	if(Device_Found){
		
		//Performance Vs. Power consumption XY (medium), and set data rate to 10Hz
		I2C_Write_Reg(LIS3MDL_ADDRESS,LIS3MDL_CTRL_REG1,(LIS3MDL_CTRL_REG1_OM0 | LIS3MDL_CTRL_REG1_DO2));
		
		//Performance Vs. Power consumption Z (medium)
		I2C_Write_Reg(LIS3MDL_ADDRESS,LTS3MDL_CTRL_REG4,LIS3MDL_CTRL_REG4_OMZ0);
		
		//Enable BDU so you ensure MSB and LSB have been read
		I2C_Write_Reg(LIS3MDL_ADDRESS,LTS3MDL_CTRL_REG5,LIS3MDL_CTRL_REG5_BDU);
		
		//Set device to single conversion mode
		I2C_Write_Reg(LIS3MDL_ADDRESS,LTS3MDL_CTRL_REG3,0x0);
	}
	
	return(Device_Found);
}

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
	printf("---------------------------------------------------------\r\n");
}

float LIS2MDL_XYZ_Read(void){
	
}
