#include "stm32l053xx.h"                  // Device header
#include <stdio.h>
#include "I2C.h"													// I2C Support
#include "HTS221.h"
#include "Serial.h"
#include "Timing.h"

/*------------------------------------Slave Address-------------------------------------------*/
#define HTS221_ADDRESS  	0x0000005F 		//Slave Address	for temp humidity sensor (WITHOUT R/W)

/*------------------------------------HTS221 Registers----------------------------------------*/
#define HTS221_AV_CONF  					0x10					//Accuracy configuration Register
#define HTS221_WHO_AM_I					0x0F					//Who am I register on HTS221
#define	HTS221_DEVICE_ID					0xBC					//The ID of the device (different from slave address)
#define HTS221_TEMP_OUT_L				0x2A					//Temperature Data (LSB)
#define HTS221_TEMP_OUT_H				0x2B					//Temperatuer Data (MSB)
#define HTS221_CTRL_REG1					0x20					//PD(Power Down),BDU(Block Data Output)
#define HTS221_CTRL_REG2					0x21					//Boot,heater and one-shot configuration
#define HTS221_STATUS_REG				0x27					//Status of Temperature and Humidity readings
#define HTS221_HUMIDITY_OUT_L		0x28					//Humidity Data (LSB)
#define HTS221_HUMIDITY_OUT_H		0x29					//Humidity Data (MSB)
#define HTS221_T0_degC_x8				0x32					//Temperature Calibration lower
#define HTS221_T1_degC_x8				0x33					//Temperatuer Calibration upper
#define HTS221_T1_T0_Msb				0x35					//Temperatuer Calibration MSB bits since a 10-bit number
/*-------------------------------------HTS221 Configuration Bits------------------------------*/
#define HTS221_STATUS_REG_TDA				0x00000001						//Temperature Data Available
#define HTS221_STATUS_REG_HDA				0x00000002						//Humidity data available
#define HTS221_CTRL_REG2_ONE_SHOT		0x00000001						//Single Acquisition of Temperature and Humidity when 1
#define HTS221_CTRL_REG1_PD					0x00000080						//Power Down, 0 = Power down mode, 1 = active mode
#define HTS221_CTRL_REG1_BDU				0x00000004						//Block Data Output, 0 continuous update, 1 wait until LSB and MSB Read

/*-------------------------------------Globals------------------------------------------------*/
uint32_t AV_CONF_Init = 0x1B;		// 16 Temp (AVGT) and 32 Hum (AVGT)
uint16_t T_OUT_L = 0;
uint16_t T_OUT_H = 0;
uint8_t T0_degC_x8 = 0;
uint8_t T1_degC_x8 = 0;
uint16_t Msb_T0_degC = 0;
uint16_t Msb_T1_degC = 0;
uint16_t Msb_TO_T1_degC = 0;
int16_t T_OUT = 0;
/*--------------------------------------------------------------------------------------------*/

void HTS221_Temp_Read(void){
	
	uint16_t Calibration_Initial = 0;
	uint16_t Calibration_Final = 0;
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_TEMP_OUT_L);
	T_OUT_L = I2C1->RXDR;
	printf("Temperature Low: %x\r\n",T_OUT_L);
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_TEMP_OUT_H);
	T_OUT_H = I2C1->RXDR;
	printf("Temperature High: %x\r\n",T_OUT_H);
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_T0_degC_x8);
	T0_degC_x8 = I2C1->RXDR;
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_T1_degC_x8);
	T1_degC_x8 = I2C1->RXDR;
	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_T1_T0_Msb);
	Msb_TO_T1_degC = I2C1->RXDR;
	
	Calibration_Initial = ((Msb_T0_degC & 0x2)<<8) | (T0_degC_x8);
	Calibration_Final = ((Msb_T1_degC & 0xC)<<8) | (T1_degC_x8);
	
	printf("Initial Calibration: %i\r\n",Calibration_Initial);
	printf("Final Calibration: %i\r\n",Calibration_Final);

	//T_OUT = (T_OUT_H<<8) | (T_OUT_L);
}

void HTS221_Configuration(void){
	
	//HTS221_AV_CONF Settings
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_AV_CONF);
	printf("HTS221_AV_CONF: %x\r\n",I2C1->RXDR);
	
	//HTS221_CTRL_REG1 Settings
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_CTRL_REG1);
	printf("HTS221_CTRL_REG1: %x\r\n",I2C1->RXDR);
	
	//HTS221_CTRL_REG2 Settings
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_CTRL_REG2);
	printf("HTS221_CTRL_REG2: %x\r\n",I2C1->RXDR);
}

void HTS221_Init(void){
	
	/*------------------Check the Device ID------------------------*/
	
	//Read data from register and check signature	
	I2C_Read_Reg(HTS221_ADDRESS,HTS221_WHO_AM_I);
	
	//Check if device signature is correct
	if (I2C1->RXDR == HTS221_DEVICE_ID){
		printf("!!HTS221 Found!!\r\n");
	}
	else{
		printf("Device Not Found, ID: %x\r\n",I2C1->RXDR);
		HTS221_Init();		//If device is incorrect try again
	}
	/*------------------------------------------------------------*/
	
	/*----------------Setup HTS221_AV_CONF Register----------------------*/
	I2C_Write_Reg(HTS221_ADDRESS,HTS221_AV_CONF,AV_CONF_Init);
	/*------------------------------------------------------------*/
	I2C_Write_Reg(HTS221_ADDRESS,HTS221_CTRL_REG1,(HTS221_CTRL_REG1_PD | HTS221_CTRL_REG1_BDU));
	/*------------------------------------------------------------*/
	I2C_Write_Reg(HTS221_ADDRESS,HTS221_CTRL_REG2,HTS221_CTRL_REG2_ONE_SHOT);
}
