#include "stm32l053xx.h"                  // Device header
#include <stdio.h>
#include "I2C.h"													// I2C Support
#include "Serial.h"
#include "LPS25HB.h"

/*---------------------------------Slave Address---------------------------------*/
#define LPS25HB_ADDRESS 0x5D		//Note that SA0 = 1 so address is 1011101 and not 1011100
/*---------------------------------Device ID----------------------------------------------*/
#define LPS25HB_DEVICE_ID				0xBD	//Device ID, the value in the WHO_AM_I 	Register
/*---------------------------------Register Locations----------------------------*/
#define LPS25HB_WHO_AM_I				0x0F	//Who am I register location
#define LPS25HB_STATUS_REG			0x27	//Tells whether the Pressure Data is ready or is being overrun
#define LPS25HB_PRESS_OUT_XL		0x28	//(LSB) Pressure output value
#define LPS25HB_PRESS_OUT_L			0x29  //(mid part) Pressure output value
#define LPS25HB_PRESS_OUT_H			0x2A	//(MSB) Pressure output value
#define LPS25HB_CTRL_REG1				0x20	//Contains PD, BDU and more
#define LPS25HB_CTRL_REG2				0x21	//Contains one-shot mode and FIFO settings
#define LPS25HB_RES_CONF				0x10	//Pressure and temperature Resolution
/*---------------------------------Configuration Bits------------------------------*/
#define LPS25HB_CTRL_REG1_PD					0x80	//Power Down when 0, active mode when 1 (Default 0)
#define LPS25HB_CTRL_REG1_BDU					0x4		//Block Data Update: 0 Continuous mode, 1 read LSB,Mid,MSB first
#define LPS25HB_CTRL_REG2_ONE_SHOT		0x1		//One shot mode enabled, obtains a new dataset
#define LPS25HB_RES_CONF_AVGP0				0x1		//Pressure resolution Configuration
#define LPS25HB_RES_CONF_AVGP1				0x2		//Pressure resolution Configuration
#define LPS25HB_STATUS_REG_PDA				0x2		//Pressure data available
/*---------------------------------Global Variables--------------------------------*/
//Initial Pressure readings
uint8_t PRESS_OUT_XL = 0;
uint8_t PRESS_OUT_L = 0;
uint8_t PRESS_OUT_H = 0;

//Pressure Variables
float LPS25HB_Pressure = 0;

// Determines if Temperature or Humidity Data is ready
uint8_t LPS25HB_STATUS = 0;

uint8_t LPS25HB_Init(void){
	
	uint8_t Device_Found = 0;
	
	//Read data from register and check signature	
	I2C_Read_Reg(LPS25HB_ADDRESS,LPS25HB_WHO_AM_I);
	
	//Check if device signature is correct
	if (I2C1->RXDR == LPS25HB_DEVICE_ID){
		Device_Found = 1;
	}
	else{
		Device_Found = 0;
	}
	
	if(Device_Found){
		//Power on the device and Block Data Update
		I2C_Write_Reg(LPS25HB_ADDRESS,LPS25HB_CTRL_REG1,(LPS25HB_CTRL_REG1_PD | LPS25HB_CTRL_REG1_BDU));
		
		//Configure the resolution for pressure for 16 internal averages
		I2C_Write_Reg(LPS25HB_ADDRESS,LPS25HB_RES_CONF,LPS25HB_RES_CONF_AVGP0);
	}
	
	return(Device_Found);
}

void LPS25HB_Configuration(void){
	
	printf("----------------Configuration Settings-------------------\r\n");	
	//Resolution Settings
	I2C_Read_Reg(LPS25HB_ADDRESS,LPS25HB_RES_CONF);
	printf("Resolution Settings: %x\r\n",I2C1->RXDR);
	
	//HTS221_CTRL_REG1 Settings
	I2C_Read_Reg(LPS25HB_ADDRESS,LPS25HB_CTRL_REG1);
	printf("LPS25HB_CTRL_REG1: %x\r\n",I2C1->RXDR);
	printf("---------------------------------------------------------\r\n");
}

float LPS25HB_Pressure_Read(void){
	
	int32_t Raw_Pressure = 0;
	
	//Start a temperature conversion
	I2C_Write_Reg(LPS25HB_ADDRESS,LPS25HB_CTRL_REG2,LPS25HB_CTRL_REG2_ONE_SHOT);
	
	//Wait for Temperature data to be ready
	do{
		I2C_Read_Reg(LPS25HB_ADDRESS,LPS25HB_STATUS_REG);
		LPS25HB_STATUS = I2C1->RXDR;
	}while((LPS25HB_STATUS & LPS25HB_STATUS_REG_PDA) == 0);
	
	//Read the pressure output registers
	I2C_Read_Reg(LPS25HB_ADDRESS,LPS25HB_PRESS_OUT_XL);
	PRESS_OUT_XL = I2C1->RXDR;
	
	I2C_Read_Reg(LPS25HB_ADDRESS,LPS25HB_PRESS_OUT_L);
	PRESS_OUT_L = I2C1->RXDR;
	
	I2C_Read_Reg(LPS25HB_ADDRESS,LPS25HB_PRESS_OUT_H);
	PRESS_OUT_H = I2C1->RXDR;
	
	//Read the reference Register
	
	/*	Combine pressure into 24 bit value
			PRESS_OUT_H is the High bits 	23 - 16
			PRESS_OUT_L is the mid bits 	15 - 8
			PRESS_OUT_XL is the lsb				7 - 0
	*/
	Raw_Pressure = ((PRESS_OUT_H << 16) | (PRESS_OUT_L << 8) | (PRESS_OUT_XL));
	
	//convert the 2's complement 24 bit to 2's complement 32 bit
	if (Raw_Pressure & 0x00800000){
			Raw_Pressure |= 0xFF000000;
	}
		
	LPS25HB_Pressure = (float)Raw_Pressure/4096.0f;
	
	return(LPS25HB_Pressure);
}
