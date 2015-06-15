#include <stdio.h>
#include "stm32l0xx.h"                  // Device header
#include "GPIO.h"
#include "Timing.h"
#include "Serial.h"
#include "Gpio_Test.h"

/**
  \fn         void User_Button_Led_Check(void)
  \brief       Runs a simple program that blinks LD2 and pauses when User button pressed
*/

void User_Button_Led_Check(void){

	//Initializations
	//LED_Init();
	
	//Operation
	//Button_Initialize();
	
	LED_On();
  Delay(500);
	while (Button_Get_State() & (1 << 0));
	
	LED_Off();
  Delay(500);
	while (Button_Get_State() & (1 << 0));
}
