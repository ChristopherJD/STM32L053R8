/*----------------------------------------------------------------------------
 * Name:    Intern_Project.c
 * Purpose: Awesome Project
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2015 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/
 
 /*-----------------------Include Statements----------------------------------*/
#include <stdio.h>
#include "stm32l0xx.h"                  // Device header
#include "stm32l053xx.h"
#include "Serial.h"											// Serial Communication
#include "GPIO.h"												// Drivers for GPIO
#include "Timing.h"											// Drivers for clock
#include "Gpio_Test.h"									// Gpio testing functions
#include "ADC.h"												// ADC initialization
#include "ADC_Test.h"										// ADC testing function
#include "I2C.h"												// I2C initialization
#include "HTS221.h"											// Temperature and humidity Drivers
#include "LPS25HB.h"										// Pressure sensor Drivers
#include "LIS3MDL.h"										// Magnetometer drivers

/*------------------------Definitions-----------------------------------------*/

/*------------------------Global Variables------------------------------------*/
	//Sensor output variables
	float Temperature = 0;
	float Humidity = 0;
	float Pressure = 0;
	float X_Coordinate = 0;
	float Y_Coordinate = 0;
	float Z_Coordinate = 0;
	
	//Found Devices Variables
	uint8_t HTS221_Found = 0;
	uint8_t LPS25HB_Found = 0;
	uint8_t LIS3MDL_Found = 0;
/*----------------------------------------------------------------------------
 * main: blink LED 
 *----------------------------------------------------------------------------*/
int main (void) {
	
	// configure HSI as System Clock
	SystemCoreClockInit();
  SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);									// SysTick 1 msec interrupts

  // Port initializations
	LED_Init();																							//LD2 Initialization
	Button_Initialize();																		//User button init
	
	// Serial Communications Initializations
  SER_Initialize();
	
	//ADC Initializations
	ADC_Init();
	
	//I2C Initialization
	I2C_Init();
	
	//Temperature Sensor Initialize
	HTS221_Found = HTS221_Init();		//Initializes the device if found
	if(HTS221_Found){
		printf("#####  HTS221 Found  #####\r\n");
	}
	else printf("#####  HTS221 Not Connected  #####\r\n");
	HTS221_Configuration();		//Prints the configuration
	
	//Pressure Sensor Initialize
	LPS25HB_Found = LPS25HB_Init();
	if(LPS25HB_Found){
		printf("#####  LPS25HB Found  #####\r\n");
	}
	else printf("#####  LPS25HB Not Connected  #####\r\n");
	LPS25HB_Configuration();		//Prints the configuration
	
	//Magnetometer Initialize
	LIS3MDL_Found = LIS3MDL_Init();
	if(LIS3MDL_Found){
		printf("#####  LIS3MDL Found  #####\r\n");
	}
	else printf("#####  LIS3MDL Not Connected  #####\r\n");
	LIS3MDL_Configuration();
	
	//Loop Forever
  while (1) {
//		Temperature = HTS221_Temp_Read();
//		Humidity = HTS221_Humidity_Read();
//		Pressure = LPS25HB_Pressure_Read();
//		printf("Temperature: %f %cF\r\n",Temperature,248);
//		printf("Humidity: %f rH\r\n",Humidity);
//		printf("Pressure: %f hPa\r\n",Pressure);
		X_Coordinate = LIS2MDL_X_Read();
		printf("X coordinate: %f mG\r\n",X_Coordinate);
		
		Y_Coordinate = LIS2MDL_Y_Read();
		printf("Y coordinate: %f mG\r\n",Y_Coordinate);
		
		Z_Coordinate = LIS2MDL_Z_Read();
		printf("Z coordinate: %f mG\r\n",Z_Coordinate);
		Delay(250);
  }

}
