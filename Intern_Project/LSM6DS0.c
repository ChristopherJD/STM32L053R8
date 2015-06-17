#include "stm32l053xx.h"                  // Device header
#include <stdio.h>
#include "I2C.h"													// I2C Support
#include "Serial.h"												// USART Communication
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
#define LSM6DS0_CTRL_REG8_BDU		0x40		//BDU Enable
#define LSM6DS0_STATUS_REG_XLDA		0x1			//Acceleration Data available
#define LSM6DS0_STATUS_REG_GDA		0x2			//Gyroscope Data Available
#define LSM6DS0_CTRL_REG1_G_ODR_G0		0x20		//Gyroscope output data rate
#define LSM6DS0_CTRL_REG1_G_ODR_G1		0x40		//Gyroscope output data rate
#define LSM6DS0_CTRL_REG1_G_ODR_G2		0x80		//Gyroscope output data rate

uint8_t LSM6DS0_Init(void){
	//Global Variables
	uint8_t Device_Found = 0;
	
	/*---------------------------------Check the Device ID--------------------------------------------*/
	
	//Read data from register and check signature	
	I2C_Read_Reg(LSM6DS0_ADDRESS,LSM6DS0_WHO_AM_I);
	
	//Check if device signature is correct
	if (I2C1->RXDR == LSM6DS0_DEVICE_ID){
		Device_Found = 1;
	}
	else Device_Found = 0;
	
	/*-------------------------------Setup HTS221_AV_CONF Register------------------------------------*/
	if(Device_Found){
		
		//Activate both the gyro and the accelerometer at the same ODR of 238 Hz
		I2C_Write_Reg(LSM6DS0_ADDRESS,LSM6DS0_CTRL_REG1_G,LSM6DS0_CTRL_REG1_G_ODR_G2);
	}
	/*------------------------------------------------------------------------------------------------*/
	
	return(Device_Found);
}
void LSM6DS0_Configuration(void){
	
}
