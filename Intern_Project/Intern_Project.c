/*----------------------------------------------------------------------------
 * Name:    Intern_Project.c
 * Purpose: Demonstrate the STM32L053 Nucleo Board and its capabilities
 * Date: 		6/18/15
 * Author:	Christopher Jordan - Denny
 *----------------------------------------------------------------------------
 * Note(s):
 *----------------------------------------------------------------------------*/
 
 /*-----------------------Include Statements----------------------------------*/
#include <stdio.h>											// Standard Input Output
#include "stm32l053xx.h"								// Specific Device Header
#include "Serial.h"											// Usart2 Communication
#include "USART1.h"											// Usart1 Communication
#include "GPIO.h"												// GPIO Drivers
#include "Timing.h"											// Clock Drivers
#include "ADC.h"												// ADC Drivers
#include "I2C.h"												// I2C Drivers
#include "ISK01A1.h"										// ISK01A1 expansion board Drivers (gryo,temp,accel etc...)
	
/**
  \fn          int main (void)
  \brief       Initializes all peripherals and loops forever
*/

int main (void){
	
	// configure HSI as System Clock
	SystemCoreClockInit();
  SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);									// SysTick 1 msec interrupts

  // Port initializations
	LED_Init();																							//LD2 Initialization
	Button_Initialize();																		//User button init
	
	// Serial Communications Initializations
  SER_Initialize();
	USART1_Init();
	
	//ADC Initializations
	ADC_Init();
	
	//I2C Initialization
	I2C_Init();
	
	//Mems board Initialization
	//ISK01A1_Init();
	
	//USART1_Read();
	
	//Loop Forever
  while (1) {
		//printf("Acc_Y: %f\r\n",ISK01A1_Get_Temperature());
		//printf("%i",USART1_GetChar());
		//SER_PutChar(0xD5);
		
		USART1_PutChar('a');
		printf("%c",USART1_GetChar());
		
		Delay(250);
  }

}
