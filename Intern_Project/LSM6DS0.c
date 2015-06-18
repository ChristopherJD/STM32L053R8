/*----------------------------------------------------------------------------
 * Name:    LSM6DS0.c
 * Purpose: Retrieve gyroscope and acceleration data
 * Date: 		6/18/15
 * Author:	Christopher Jordan - Denny
 *----------------------------------------------------------------------------
 * Note(s): Sensor communicates using I2C.Reads gyro and acceleration data
 *----------------------------------------------------------------------------*/
 
/*----------------------------------------------Include Statements----------------------------------*/
#include "stm32l053xx.h"                  // Specific device header
#include <stdio.h>												// Standard Input Output
#include "I2C.h"													// I2C Drivers
#include "Serial.h"												// USART Drivers
#include "LSM6DS0.h"
/*------------------------------------Device Slave Address------------------------------------------*/
#define LSM6DS0_ADDRESS			0x6B		//The slave address of the device without r/w
/*------------------------------------Device ID-----------------------------------------------------*/
#define LSM6DS0_DEVICE_ID		0x68		//The value in the who am I register
/*------------------------------------Registers-----------------------------------------------------*/
#define LSM6DS0_WHO_AM_I		0x0F		//Contains the device ID
#define LSM6DS0_CTRL_REG1_G		0x10		//Contains output data rate
#define LSM6DS0_CTRL_REG2_G		0x11		//Contains INT and OUT select for angular acceleration
#define LSM6DS0_CTRL_REG8			0x22		//Contains BDU (Block Data Update)
#define LSM6DS0_STATUS_REG		0x17		//Status of Interrupts and if new data is available
#define LSM6DS0_OUT_X_G_L			0x18		//LSB of angular rate
#define LSM6DS0_OUT_X_G_H			0x19		//MSB of angular rate
#define LSM6DS0_OUT_Y_G_L			0x1A		//LSB of angular rate
#define LSM6DS0_OUT_Y_G_H			0x1B		//MSB of angular rate
#define LSM6DS0_OUT_Z_G_L			0x1C		//LSB of angular rate
#define LSM6DS0_OUT_Z_G_H			0x1D		//MSB of angular rate
#define LSM6DS0_OUT_X_XL_L		0x28		//LSB Linear Acceleration
#define LSM6DS0_OUT_X_XL_H		0x29		//MSB Linear Acceleration
#define LSM6DS0_OUT_Y_XL_L		0x2A		//LSB Linear Acceleration
#define LSM6DS0_OUT_Y_XL_H		0x2B		//MSB Linear Acceleration
#define LSM6DS0_OUT_Z_XL_L		0x2C		//LSB Linear Acceleration
#define LSM6DS0_OUT_Z_XL_H		0x2D		//MSB Linear Acceleration
/*------------------------------------Register Control bits------------------------------------------*/
#define LSM6DS0_CTRL_REG8_BDU					0x40		//BDU Enable
#define LSM6DS0_STATUS_REG_XLDA				0x1			//Acceleration Data available
#define LSM6DS0_STATUS_REG_GDA				0x2			//Gyroscope Data Available
#define LSM6DS0_CTRL_REG1_G_ODR_G0		0x20		//Gyroscope output data rate
#define LSM6DS0_CTRL_REG1_G_ODR_G1		0x40		//Gyroscope output data rate
#define LSM6DS0_CTRL_REG1_G_ODR_G2		0x80		//Gyroscope output data rate

/*------------------------------------Functions------------------------------------------------------*/

/**
  \fn					uint8_t LSM6DS0_Init(void)
  \brief			Initialize LSM6DS0 Gyro and accelerometer
	\returns		uint8_t Device_Found: 1 - Device found, 0 - Device not found	
*/

uint8_t LSM6DS0_Init(void){
	
	//Global Variables
	uint8_t Device_Found = 0;
	
	//Read data from register and check signature	
	I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_WHO_AM_I);
	
	//Check if device signature is correct
	if (I2C1->RXDR == LSM6DS0_DEVICE_ID){
		Device_Found = 1;
	}
	else Device_Found = 0;
	
	if(Device_Found){
		
		//Enable Block Data Update until MSB and LSB read
		I2C_Write_Reg(LSM6DS0_ADDRESS,LSM6DS0_CTRL_REG8,LSM6DS0_CTRL_REG8_BDU);
		
		//Activate both the gyro and the accelerometer at the same ODR of 238 Hz
		I2C_Write_Reg(LSM6DS0_ADDRESS,LSM6DS0_CTRL_REG1_G,LSM6DS0_CTRL_REG1_G_ODR_G2);
	}
	
	return(Device_Found);
}

/**
  \fn					void LSM6DS0_Configuration(void)
  \brief			Prints important configuration registers
*/

void LSM6DS0_Configuration(void){
	
	printf("----------------Configuration Settings-------------------\r\n");	
	
	I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_CTRL_REG8);
	printf("CTRL_Reg8: %x\r\n",I2C1->RXDR);
	
	I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_CTRL_REG1_G);
	printf("CTRL_Reg1_G: %x\r\n",I2C1->RXDR);
	
	printf("---------------------------------------------------------\r\n");
}

/**
  \fn					float LSM6DS0_X_Acceleration_Read(void)
  \brief			Retrieves X-Direction Acceleration
	\returns		float Acceleration_X: Acceleration in mg
*/

float LSM6DS0_X_Acceleration_Read(void){
	
	//Local Variables
	uint8_t LSM6DS0_STATUS = 0;
	uint8_t Out_X_XL_L = 0;
	uint8_t Out_X_XL_H = 0;
	int16_t Raw_X = 0;
	float Acceleration_X = 0;
	
	//Wait for acceleration data to be ready
	do{
		I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_STATUS_REG);
		LSM6DS0_STATUS = I2C1->RXDR;
	}while((LSM6DS0_STATUS & LSM6DS0_STATUS_REG_XLDA) == 0);
	
	//Read acceleration output registers
	I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_OUT_X_XL_L);
	Out_X_XL_L = I2C1->RXDR;
	
	I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_OUT_X_XL_H);
	Out_X_XL_H = I2C1->RXDR;
	
	//Combine Lower and upper bits
	Raw_X = ((Out_X_XL_H << 8) | Out_X_XL_L);
	
	//Calculate acceleration based on FS configuration, see datasheet
	Acceleration_X = (float)Raw_X*0.061f;
	
	return(Acceleration_X);
}

/**
  \fn					float LSM6DS0_Y_Acceleration_Read(void)
  \brief			Retrieves Y-Direction Acceleration
	\returns		float Acceleration_Y: Acceleration in mg
*/

float LSM6DS0_Y_Acceleration_Read(void){
	
	//Local Variables
	uint8_t LSM6DS0_STATUS = 0;
	uint8_t Out_Y_XL_L = 0;
	uint8_t Out_Y_XL_H = 0;
	int16_t Raw_Y = 0;
	float Acceleration_Y = 0;
	
	//Wait for acceleration data to be ready
	do{
		I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_STATUS_REG);
		LSM6DS0_STATUS = I2C1->RXDR;
	}while((LSM6DS0_STATUS & LSM6DS0_STATUS_REG_XLDA) == 0);
	
	//Read acceleration output registers
	I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_OUT_Y_XL_L);
	Out_Y_XL_L = I2C1->RXDR;
	
	I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_OUT_Y_XL_H);
	Out_Y_XL_H = I2C1->RXDR;
	
	//Combine Lower and upper bits
	Raw_Y = ((Out_Y_XL_H << 8) | Out_Y_XL_L);
	
	//Calculate acceleration based on FS configuration, see datasheet
	Acceleration_Y = (float)Raw_Y*0.061f;
	
	return(Acceleration_Y);
}

/**
  \fn					float LSM6DS0_Z_Acceleration_Read(void)
  \brief			Retrieves Z-Direction Acceleration
	\returns		float Acceleration_Z: Acceleration in mg
*/

float LSM6DS0_Z_Acceleration_Read(void){
	
	//Local Variables
	uint8_t LSM6DS0_STATUS = 0;
	uint8_t Out_Z_XL_L = 0;
	uint8_t Out_Z_XL_H = 0;
	int16_t Raw_Z = 0;
	float Acceleration_Z = 0;
	
	//Wait for acceleration data to be ready
	do{
		I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_STATUS_REG);
		LSM6DS0_STATUS = I2C1->RXDR;
	}while((LSM6DS0_STATUS & LSM6DS0_STATUS_REG_XLDA) == 0);
	
	//Read acceleration output registers
	I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_OUT_Z_XL_L);
	Out_Z_XL_L = I2C1->RXDR;
	
	I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_OUT_Z_XL_H);
	Out_Z_XL_H = I2C1->RXDR;
	
	//Combine the lower and upper bits
	Raw_Z = ((Out_Z_XL_H << 8) | Out_Z_XL_L);
	
	//Calculate acceleration based on FS configuration, see datasheet
	Acceleration_Z = (float)Raw_Z*0.061f;
	
	return(Acceleration_Z);
}

/**
  \fn					float LSM6DS0_Gyroscope_Roll_Read(void)
  \brief			Retrieves X-Direction(Roll)
	\returns		float Roll: Roll in mdps
*/

float LSM6DS0_Gyroscope_Roll_Read(void){
	
	//Local Variables
	uint8_t LSM6DS0_STATUS = 0;
	uint8_t Roll_L = 0;
	uint8_t Roll_H = 0;
	int16_t Raw_Roll = 0;
	float Roll = 0;
	
	//Wait for roll data to be ready
	do{
		I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_STATUS_REG);
		LSM6DS0_STATUS = I2C1->RXDR;
	}while((LSM6DS0_STATUS & LSM6DS0_STATUS_REG_GDA) == 0);
	
	//Read Gyroscope output registers
	I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_OUT_X_G_L);
	Roll_L = I2C1->RXDR;
	
	I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_OUT_X_G_H);
	Roll_H = I2C1->RXDR;
	
	//Combine lower and upper bits
	Raw_Roll = ((Roll_H << 8) | Roll_L);
	
	//Calculate Roll based on FS configuration,see datasheet
	Roll = (float)Raw_Roll*8.75f;
	
	return(Roll);
}

/**
  \fn					float LSM6DS0_Gyroscope_Pitch_Read(void)
  \brief			Retrieves Y-Direction(Pitch)
	\returns		float Pitch: Pitch in mdps
*/

float LSM6DS0_Gyroscope_Pitch_Read(void){
	
	//Local Variables
	uint8_t LSM6DS0_STATUS = 0;
	uint8_t Pitch_L = 0;
	uint8_t Pitch_H = 0;
	int16_t Raw_Pitch = 0;
	float Pitch = 0;
	
	//Wait for pitch data to be ready
	do{
		I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_STATUS_REG);
		LSM6DS0_STATUS = I2C1->RXDR;
	}while((LSM6DS0_STATUS & LSM6DS0_STATUS_REG_GDA) == 0);
	
	//Read gyroscope output registers
	I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_OUT_Y_G_L);
	Pitch_L = I2C1->RXDR;
	
	I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_OUT_Y_G_H);
	Pitch_H = I2C1->RXDR;
	
	//Combine lower and upper bits
	Raw_Pitch = ((Pitch_H << 8) | Pitch_L);
	
	//Calculate Pitch based on FS configuration,see datasheet 
	Pitch = (float)Raw_Pitch*8.75f;
	
	return(Pitch);
}

/**
  \fn					float LSM6DS0_Gyroscope_Yaw_Read(void)
  \brief			Retrieves Z-Direction(Yaw)
	\returns		float Yaw: Yaw in mdps
*/

float LSM6DS0_Gyroscope_Yaw_Read(void){
	
	//Local Variables
	uint8_t LSM6DS0_STATUS = 0;
	uint8_t Yaw_L = 0;
	uint8_t Yaw_H = 0;
	int16_t Raw_Yaw = 0;
	float Yaw = 0;
	
	//Wait for Yaw data to be ready
	do{
		I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_STATUS_REG);
		LSM6DS0_STATUS = I2C1->RXDR;
	}while((LSM6DS0_STATUS & LSM6DS0_STATUS_REG_GDA) == 0);
	
	//Read gyroscope output registers
	I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_OUT_Z_G_L);
	Yaw_L = I2C1->RXDR;
	
	I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_OUT_Z_G_H);
	Yaw_H = I2C1->RXDR;
	
	//Combine lower and upper bits
	Raw_Yaw = ((Yaw_H << 8) | Yaw_L);
	
	//Calculate Yaw based on FS configuration,see datasheet 
	Yaw = (float)Raw_Yaw*8.75f;
	
	return(Yaw);
}
