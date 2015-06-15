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
#include "Serial.h"											// Serial Communication
#include "GPIO.h"												// Drivers for GPIO
#include "Timing.h"											// Drivers for clock
#include "Gpio_Test.h"									// Gpio testing functions
#include "ADC.h"												// ADC initialization
#include "ADC_Test.h"										// ADC testing function
#include "I2C.h"												// I2C initialization
#include "HTS221.h"											// HTS221 Drivers
#include "LPS25HB.h"										// LPS25HB Drivers

/*------------------------Definitions-----------------------------------------*/

/*------------------------Global Variables------------------------------------*/
	float Temperature = 0;
	float Humidity = 0;
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
	HTS221_Init();
	
	//Pressure Sensor Initialize
	//LPS25HB_Init();

	//Loop Forever
  while (1) {
		Temperature = HTS221_Temp_Read();
		Humidity = HTS221_Humidity_Read();
		printf("Temperature: %f\r\n",Temperature);
		printf("Humidity: %f\r\n",Humidity);
		Delay(250);
  }

}
