#include <stdio.h>
#include "stm32l0xx.h"                  // Device header
#include "stm32l053xx.h"
#include "Serial.h"											// Serial Communication
#include "Timing.h"											// Drivers for clock
#include "HTS221.h"											// Temperature and humidity Drivers
#include "LPS25HB.h"										// Pressure sensor Drivers
#include "LIS3MDL.h"										// Magnetometer drivers
#include "LSM6DS0.h"										// Accelerometer and gyroscope

/*------------------------------------Global Variables----------------------------------*/

//Found Devices Variables
uint8_t HTS221_Found = 0;
uint8_t LPS25HB_Found = 0;
uint8_t LIS3MDL_Found = 0;
uint8_t LSM6DS0_Found = 0;

void ISK01A1_Init(void){
	
	//Temperature Sensor Initialize
	HTS221_Found = HTS221_Init();		//Initializes the device if found
	
	//Pressure Sensor Initialize
	LPS25HB_Found = LPS25HB_Init();
	
	//Magnetometer Initialize
	LIS3MDL_Found = LIS3MDL_Init();
	
	//Accelerometer and Gyroscope Initialize
	LSM6DS0_Found = LSM6DS0_Init();
	
	if(HTS221_Found & LPS25HB_Found & LIS3MDL_Found & LSM6DS0_Found){
		printf("#####  ISK01A1 Devices Initialized  #####\r\n");
	}
	else{
		
		if(HTS221_Found){
			printf("#####  HTS221 Found  #####\r\n");
		}
		else printf("#####  HTS221 Not Connected  #####\r\n");
		
		if(LPS25HB_Found){
			printf("#####  LPS25HB Found  #####\r\n");
		}
		else printf("#####  LPS25HB Not Connected  #####\r\n");

		if(LIS3MDL_Found){
			printf("#####  LIS3MDL Found  #####\r\n");
		}
		else printf("#####  LIS3MDL Not Connected  #####\r\n");
		
		if(LSM6DS0_Found){
			printf("#####  LSM6DS0 Found  #####\r\n");
		}
		else printf("#####  LSM6DS0 Not Connected  #####\r\n");
	}
}

void ISK01A1_Configuration(void){
	HTS221_Configuration();		//Prints the configuration
	LPS25HB_Configuration();	//Prints the configuration
	LIS3MDL_Configuration();	//Prints the configuration
	LSM6DS0_Configuration();	//Prints the configuration
}
