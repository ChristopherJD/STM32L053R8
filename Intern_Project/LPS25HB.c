#include "stm32l053xx.h"                  // Device header
#include <stdio.h>
#include "I2C.h"													// I2C Support
#include "Serial.h"
#include "LPS25HB.h"

/*---------------------------------Slave Address---------------------------------*/
#define LPS25HB_ADDRESS 0x5D		//Note that SA0 = 1 so address is 1011101 and not 1011100

/*---------------------------------Register Locations----------------------------*/
#define LPS25HB_WHO_AM_I 0x0F		//Who am I register location
#define LPS25HB_DEVICE_ID 0xBD	//Device ID, the value in the WHO_AM_I 	Register
#define LPS25HB_STATUS_REG 0x27	//

void LPS25HB_Init(void){
	
	//Read data from register and check signature	
	I2C_Read_Reg(LPS25HB_ADDRESS,LPS25HB_WHO_AM_I);
	
	//Check if device signature is correct
	if (I2C1->RXDR == LPS25HB_DEVICE_ID){
		printf("!!LPS25HB Found!!\r\n");
	}
	else{
		printf("Device Not Found, ID: %x\r\n",I2C1->RXDR);
		LPS25HB_Init();		//If device is incorrect try again
	}
	
	
}
