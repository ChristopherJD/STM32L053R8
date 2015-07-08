/*----------------------------------------------------------------------------
 * Name:    Intern_Project.c
 * Purpose: Demonstrate the STM32L053 Nucleo Board and its capabilities
 * Date: 		6/18/15
 * Author:	Christopher Jordan - Denny
 *----------------------------------------------------------------------------
 * Note(s):
 *----------------------------------------------------------------------------*/
 
 /*-----------------------Include Statements-----------------------------------------------------------*/
#include <stdio.h>											// Standard Input Output
#include "stm32l053xx.h"								// Specific Device Header
#include "Serial.h"											// Usart2 Communication
#include "FGPMMOPA6H.h"									// GPS Drivers
#include "GPIO.h"												// GPIO Drivers
#include "Timing.h"											// Clock Drivers
#include "ADC.h"												// ADC Drivers
#include "I2C.h"												// I2C Drivers
#include "ISK01A1.h"										// ISK01A1 expansion board Drivers (gryo,temp,accel etc...)
#include "XBeePro24.h"
#include "string.h"
/*-----------------------Functions----------------------------------------------------------------------*/
void IO_Init(void);

/**
  \fn          int main (void)
  \brief       Initializes all peripherals and loops forever
*/

int main (void){
	
	char Data[165];
	
	IO_Init();
	
	//Loop Forever
  while (1) {
		
		sprintf(Data,"%s%s",FGPMMOPA6H_Package_Data(),ISK01A1_Package_Data());
		LPUART1_Send(Data);
		Delay(5000);
  }
	
}

void IO_Init(void){
	
	// configure HSI as System Clock
	SystemCoreClockInit();
  SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);									// SysTick 1 msec interrupts
	
  // Port initializations
	LED_Init();																							//LD2 Initialization
	Button_Initialize();																		//User button init
	
	// Serial Communications Initializations
  SER_Initialize();
	USART1_Init();		/* For the GPS */
	LPUART_Init();		/* For XBee */
	
	//ADC Initializations
	ADC_Init();
	
	//I2C Initialization
	I2C_Init();
	
	//Mems board Initialization
	ISK01A1_Init();
	
	//GPS Initialization
	FGPMMOPA6H_Init();
	
	//XBee Initialization
	/* Note that setup takes 2 seconds due to 1 second delays required
	 * By the XBee AT command Sequence
	 */
	XBee_Init();
	
}
