#include "stm32l053xx.h"                  // Device header
#include <stdio.h>
#include "I2C.h"													// I2C Support
#include "HTS221.h"
#include "Serial.h"

/*------------------------------------Slave Address-------------------------------------------*/
#define HTS221_ADDRESS  	0x0000005F 		//Slave Address	for temp humidity sensor (WITHOUT R/W)

/*------------------------------------HTS221 Registers----------------------------------------*/
#define HTS221_AV_CONF  					0x10					//Accuracy configuration Register
#define HTS221_WHO_AM_I						0x0F					//Who am I register on HTS221
#define	HTS221_DEVICE_ID					0xBC					//The ID of the device (different from slave address)
#define HTS221_TEMP_OUT_L					0x2A					//Temperature Data (LSB)
#define HTS221_TEMP_OUT_H					0x2B					//Temperature Data (MSB)
#define HTS221_CTRL_REG1					0x20					//PD(Power Down),BDU(Block Data Output)
#define HTS221_CTRL_REG2					0x21					//Boot,heater and one-shot configuration
#define HTS221_STATUS_REG					0x27					//Status of Temperature and Humidity readings
#define HTS221_HUMIDITY_OUT_L			0x28					//Humidity Data (LSB)
#define HTS221_HUMIDITY_OUT_H			0x29					//Humidity Data (MSB)
#define	HTS221_H0_rH_x2						0x30					//Humidity Calibration
#define HTS221_H1_rH_x2						0x31					//Humidity Calibration
#define HTS221_T0_degC_x8					0x32					//Temperature Calibration lower
#define HTS221_T1_degC_x8					0x33					//Temperature Calibration upper
#define HTS221_T1_T0_Msb					0x35					//Temperature Calibration MSB bits since a 10-bit number
#define HTS221_H0_T0_OUT_L				0x36					//Humidity Calibration
#define HTS221_H0_T0_OUT_H				0x37					//Humidity Calibration
#define HTS221_H1_T0_OUT_L				0x3A					//Humidity Calibration
#define HTS221_H1_T0_OUT_H				0x3B					//Humidity Calibration
#define HTS221_TO_OUT_L						0x3C					//Temperatuer Calibration
#define HTS221_T0_OUT_H						0x3D					//Temperature Calibration
#define HTS221_T1_OUT_L						0x3E					//Temperature Calibration
#define HTS221_T1_OUT_H						0x3F					//Temperature Calibration
/*-------------------------------------HTS221 Configuration Bits------------------------------*/
#define HTS221_STATUS_REG_TDA				0x00000001						//Temperature Data Available
#define HTS221_STATUS_REG_HDA				0x00000002						//Humidity data available
#define HTS221_CTRL_REG2_ONE_SHOT		0x00000001						//Single Acquisition of Temperature and Humidity when 1
#define HTS221_CTRL_REG1_PD					0x00000080						//Power Down, 0 = Power down mode, 1 = active mode
#define HTS221_CTRL_REG1_BDU				0x00000004						//Block Data Output, 0 continuous update, 1 wait until LSB and MSB Read

/*-------------------------------------Globals------------------------------------------------*/
uint32_t AV_CONF_Init = 0x1B;		// 16 Temp (AVGT) and 32 Hum (AVGT)
uint8_t STATUS_REG = 0;					// Determines if Temperature or Humidity Data is ready

/*-------------------------------------Temperature Variables----------------------------------*/
//T0_degC and T1_degC
uint16_t T0_degC_x8 = 0;
uint16_t T1_degC_x8 = 0;
uint16_t Msb_T0_degC = 0;
uint16_t Msb_T1_degC = 0;
uint16_t Msb_TO_T1_degC = 0;
float T0_DegC = 0;
float T1_DegC = 0;

//T_OUT
uint16_t T_OUT_L = 0;
uint16_t T_OUT_H = 0;
float T_OUT = 0;

//T0_OUT and T1_OUT
int16_t T0_OUT_L = 0;
int16_t T0_OUT_H = 0;
int16_t T1_OUT_L = 0;
int16_t T1_OUT_H = 0;
float T0_OUT = 0;
float T1_OUT = 0;

//Temperature Variables
float Temperature_In_C = 0;
float Temperature_In_F = 0;
/*--------------------------------------Humidity Variables------------------------------------*/

//H0_rH and H1_rH
uint8_t H0_rH_x2 = 0;
float H0_rH = 0;
uint8_t H1_rH_x2 = 0;
float H1_rH = 0;

//H_OUT
float H_OUT = 0;
uint16_t H_OUT_L = 0;
uint16_t H_OUT_H = 0;

//H0_TO_OUT and H1_TO_OUT
float H0_T0_OUT = 0;
float H1_T0_OUT = 0;
uint16_t H0_T0_OUT_L = 0;
uint16_t H0_T0_OUT_H = 0;
uint16_t H1_T0_OUT_L = 0;
uint16_t H1_T0_OUT_H = 0;

//Humidity Variables
float Humidity_rH = 0;
/*--------------------------------------------------------------------------------------------*/

/**
  \fn					HTS221_Temp_Read(void)
  \brief			Reads the temperature from HTS221 in one-shot mode
	\returns		float Temperature_In_F: The temperature in Fahrenheit
*/

float HTS221_Temp_Read(void){
	
	//Start a temperature conversion
	I2C_Write_Reg(HTS221_ADDRESS,HTS221_CTRL_REG2,HTS221_CTRL_REG2_ONE_SHOT);
	
	//Wait for Temperature data to be ready
	do{
		I2C_Read_Reg(HTS221_ADDRESS,HTS221_STATUS_REG);
		STATUS_REG = I2C1->RXDR;
	}while((STATUS_REG & HTS221_STATUS_REG_TDA) == 0);
	
	//Read Temperature Data and Calibration
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_TEMP_OUT_L);
	T_OUT_L = I2C1->RXDR;
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_TEMP_OUT_H);
	T_OUT_H = I2C1->RXDR;
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_TO_OUT_L);
	T0_OUT_L = I2C1->RXDR;
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_T0_OUT_H);
	T0_OUT_H = I2C1->RXDR;
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_T1_OUT_L);
	T1_OUT_L = I2C1->RXDR;
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_T1_OUT_H);
	T1_OUT_H = I2C1->RXDR;
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_T0_degC_x8);
	T0_degC_x8 = I2C1->RXDR;
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_T1_degC_x8);
	T1_degC_x8 = I2C1->RXDR;
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_T1_T0_Msb);
	Msb_TO_T1_degC = I2C1->RXDR;
	
	//Process Calibration Registers
	T0_DegC = ((float)(((Msb_TO_T1_degC & 0x3) << 8) | (T0_degC_x8))/8.0);
	T1_DegC = ((float)(((Msb_TO_T1_degC & 0xC) << 6) | (T1_degC_x8))/8.0); //Value in 3rd and 4th bit so only shift 6
	T0_OUT = (float)((T0_OUT_H << 8) | T0_OUT_L);
	T1_OUT = (float)((T1_OUT_H << 8) | T1_OUT_L);
	T_OUT = (float)((T_OUT_H << 8) | T_OUT_L);
	
	//Calculate Temperatuer using linear interpolation and convert to Fahrenheit
	Temperature_In_C = (float)(T0_DegC + ((T_OUT - T0_OUT)*(T1_DegC - T0_DegC))/(T1_OUT - T0_OUT));
	Temperature_In_F = (Temperature_In_C*(9.0/5.0)) +32.0;
	
	return(Temperature_In_F);
}

/**
  \fn					float HTS221_Humidity_Read(void)
  \brief			Reads the Humidity from HTS221 in one-shot mode
	\returns		float Humidity_rH: The relative humidity %
*/

float HTS221_Humidity_Read(void){
	
	//Start a humidity conversion
	I2C_Write_Reg(HTS221_ADDRESS,HTS221_CTRL_REG2,HTS221_CTRL_REG2_ONE_SHOT);
	
	//Wait for Humidity data to be ready
	do{
		I2C_Read_Reg(HTS221_ADDRESS,HTS221_STATUS_REG);
		STATUS_REG = I2C1->RXDR;
	}while((STATUS_REG & HTS221_STATUS_REG_HDA) == 0);
	
	//Read Humidity data and Calibration
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_H0_rH_x2);
	H0_rH_x2 = I2C1->RXDR;
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_H1_rH_x2);
	H1_rH_x2 = I2C1->RXDR;
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_HUMIDITY_OUT_L);
	H_OUT_L = I2C1->RXDR;
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_HUMIDITY_OUT_H);
	H_OUT_H = I2C1->RXDR;
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_H0_T0_OUT_L);
	H0_T0_OUT_L = I2C1->RXDR;
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_H0_T0_OUT_H);
	H0_T0_OUT_H = I2C1->RXDR;
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_H1_T0_OUT_L);
	H1_T0_OUT_L = I2C1->RXDR;
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_H1_T0_OUT_H);
	H1_T0_OUT_H = I2C1->RXDR;
	
	//Process Calibration Registers
	H0_rH = (float)H0_rH_x2/2.0;
	H1_rH = (float)H1_rH_x2/2.0;
	H_OUT = (float)((H_OUT_H << 8) | H_OUT_L);
	H0_T0_OUT = (float)((H0_T0_OUT_H << 8) | H0_T0_OUT_L);
	H1_T0_OUT = (float)((H1_T0_OUT_H << 8) | H1_T0_OUT_L);
	
	//Calculate the relative Humidity using linear interpolation
	Humidity_rH = ( float )(((( H_OUT - H0_T0_OUT ) * ( H1_rH - H0_rH )) / ( H1_T0_OUT - H0_T0_OUT )) + H0_rH );
	
	return(Humidity_rH);
}

/**
  \fn					void HTS221_Configuration(void)
  \brief			Displays a few of the Configuration Registers,
							AV_Conf - Change Accuracy register
							CTRL_Reg1 - Power up and Block Data update
*/

void HTS221_Configuration(void){
	
	printf("----------------Configuration Settings-------------------\r\n");	
	//HTS221_AV_CONF Settings
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_AV_CONF);
	printf("HTS221_AV_CONF: %x\r\n",I2C1->RXDR);
	
	//HTS221_CTRL_REG1 Settings
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_CTRL_REG1);
	printf("HTS221_CTRL_REG1: %x\r\n",I2C1->RXDR);
	
	printf("---------------------------------------------------------\r\n");
}

/**
  \fn					void HTS221_Init(void)
  \brief			Initialize the HTS221 and check device signature
	\returns		uint8_t Device_Found - Determines if the device was detected
*/

uint8_t HTS221_Init(void){
	
	//Global Variables
	uint8_t Device_Found = 0;
	
	/*---------------------------------Check the Device ID--------------------------------------------*/
	
	//Read data from register and check signature	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_WHO_AM_I);
	
	//Check if device signature is correct
	if (I2C1->RXDR == HTS221_DEVICE_ID){
		Device_Found = 1;
	}
	else Device_Found = 0;
	
	/*-------------------------------Setup HTS221_AV_CONF Register------------------------------------*/
	if(Device_Found){
		//Set to Default Configuration
		I2C_Write_Reg(HTS221_ADDRESS,HTS221_AV_CONF,AV_CONF_Init);
		
		//Activate and Block Data Update, this will ensure that both the higher and lower bits are read
		I2C_Write_Reg(HTS221_ADDRESS,HTS221_CTRL_REG1,(HTS221_CTRL_REG1_PD | HTS221_CTRL_REG1_BDU));
	}
	/*------------------------------------------------------------------------------------------------*/
	
	return(Device_Found);
}
