/*------------------------------------------------------------------------------------------------------
 * Name:    Intern_Project.c
 * Purpose: Collects data from the GPS and ISK01A1 expansion board sensors
 * 					and sends that data using an XBee to a reciever.
 * Date: 		7/14/15
 * Author:	Christopher Jordan - Denny
 *------------------------------------------------------------------------------------------------------
 * Note(s):
 *----------------------------------------------------------------------------------------------------*/
 
 /*-----------------------Include Statements----------------------------------------------------------*/
#include <stdio.h>											// Standard Input Output
#include "stm32l053xx.h"								// Specific Device Header
#include "Serial.h"											// Usart2 Communication
#include "FGPMMOPA6H.h"									// GPS Drivers
#include "GPIO.h"												// GPIO Drivers
#include "Timing.h"											// Clock Drivers
#include "ADC.h"												// ADC Drivers
#include "I2C.h"												// I2C Drivers
#include "ISK01A1.h"										// ISK01A1 expansion board Drivers (gryo,temp,accel etc...)
#include "XBeePro24.h"									// XBee drivers
#include "string.h"											// Various useful string manipulation functions
/*-----------------------Functions--------------------------------------------------------------------*/
void IO_Init(void);

/**
  \fn          int main (void)
  \brief       Initializes all peripherals and continually fetch data
*/

int main (void){
	
	/* Local Variables */
	char Data[165];
	
	/* Initialize I2C,XBEE,ADC,USART1,USART2,LPUART1,CLOCK,ISK01A1,GPIO */
	IO_Init();
	
	/* Grab data from sensors and send */
  while (1) {
		
		/* Congregate Data */
		sprintf(Data,"%s%s",FGPMMOPA6H_Package_Data(),ISK01A1_Package_Data());
		
		/* Send data over the XBEE */
		LPUART1_Send(Data);
		
		/* Wait for GPS data, which is set to update every 5 seconds */
		Delay(5500);
  }
	
}

/**
  \fn					void IO_Init(void)
	\brief			Initializes peripherals:
							*	System Clock
							*	GPIO
							*	USART2
							* USART1
							*	LPUART1
							*	ADC
							*	I2C
							* ISK01A1 Expansion Board
							*	FGMMOPA6H Gps module
							*	XBEE Wireless communication
*/

void IO_Init(void){
	
	/* configure HSI as System Clock */
	SystemCoreClockInit();
  SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);  // SysTick 1 msec interrupts
	
  /* Port initializations */
	LED_Init();										//LD2 Initialization
	Button_Initialize();					//User button init
	
	/* Serial Communications Initializations */
  SER_Initialize();
	USART1_Init();		/* For the GPS */
	LPUART_Init();		/* For XBee */
	
	/* ADC Initializations */
	ADC_Init();
	
	/* I2C Initialization */
	I2C_Init();
	
	/* Mems board Initialization */
	ISK01A1_Init();
	
	/* GPS Initialization */
	FGPMMOPA6H_Init();
	
	/* XBee Initialization */
	/* Note that setup takes 2 seconds due to 1 second delays required
	 * By the XBee AT command Sequence
	 */
	XBee_Init();
}
