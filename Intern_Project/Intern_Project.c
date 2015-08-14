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
#include <stdlib.h>											//Various useful conversion functions
#include "stm32l053xx.h"								// Specific Device Header
#include "Serial.h"											// Usart2 Communication
#include "FGPMMOPA6H.h"									// GPS Drivers
#include "GPIO.h"												// GPIO Drivers
#include "Timing.h"											// Clock Drivers
#include "ADC.h"												// ADC Drivers
#include "I2C.h"												// I2C Drivers
#include "ISK01A1.h"										// ISK01A1 expansion board Drivers (gryo,temp,accel etc...)
#include "XBeePro24.h"									// XBee drivers
#include "PWM.h"												// Servo Motor Control
#include "Timer2.h"											// Time for parachute
#include "string.h"											// Various useful string manipulation functions

#define Green_LED  					5						// Green LED on board
#define CHUTE_DEPLOY_ALT		1619.0			// Chute deployment altitude,TRF altitude(1119) + 500 ft
/*-----------------------Structure Stuff--------------------------------------------------------------*/
RMC_Data GPS_Ready;
/*-----------------------Functions--------------------------------------------------------------------*/
void IO_Init(void);

/**
  \fn          int main (void)
  \brief       Initializes all peripherals and continually fetch data
*/

int main (void){
	
	/* Local Variables */
	char Data[256];
//	float GPS_Altitude = 0;
	
	/* Initialize I2C,XBEE,ADC,USART1,USART2,LPUART1,CLOCK,ISK01A1,GPIO */
	IO_Init();
	
	/* Wait for Button press */
	while(Button_Get_State() == 0){
		//Nop
	}
	
	/* Turn on Green LED */
	GPIO_On(GPIOA,Green_LED);
	
	/* Enable Timer */
	Start_15s_Timer();
	
	/* Grab data from sensors and send */
  while (1) {	

//		GPS_Altitude = atof(FGPMMOPA6H_Get_GGA_Altitude());
//		
//		if((FGPMMOPA6H_Get_RMC_Status() == 1) && (GPS_Altitude <= CHUTE_DEPLOY_ALT)){
//			Servo_Position(180);
//		}
		
		while(GPS_Ready.Status == 0){
			//Nop
		}
		
		/* Congregate Data */
		sprintf(Data,"%s%s",FGPMMOPA6H_Package_Data(),ISK01A1_Package_Data());
		
		/* Send data over the XBEE */
		LPUART1_Send(Data);
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
	GPIO_Output_Init(GPIOA,Green_LED);										//LD2 Initialization
	Button_Initialize();																	//User button init
	
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
	
	/* GPS Initialization with 1 second refresh rate */
	FGPMMOPA6H_Init(4);
	
	/* XBee Initialization */
	/* Note that setup takes 2 seconds due to 1 second delays required
	 * By the XBee AT command Sequence
	 */
	XBee_900HP_Init();			//Used with long distance 900 MHz XBee
//	XBee_ProS1_Init();			//Used with 2.4 GHz Xbee
	
	/*Position the servo*/
	Servo_Position(0);
}
