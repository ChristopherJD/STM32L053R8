/*------------------------------------------------------------------------------------------------------
 * Name:    ADC.c
 * Purpose: Initializes the ADC and provides ADC reading function
 * Date: 		7/14/15
 * Author:	Christopher Jordan - Denny
 *------------------------------------------------------------------------------------------------------
 * Note(s):
 *----------------------------------------------------------------------------------------------------*/

/*-----------------------------------------Include Statements-----------------------------------------*/
#include "stm32l0xx.h"		// Specific Device header
#include <stdio.h>				// Standard input output
#include "ADC.h"
/*-----------------------------------------Functions--------------------------------------------------*/

/**
  \fn          void ADC_Init(void)
  \brief       Initialize ADC and calibrate
*/

void ADC_Init(void){
	
	/* Enable ADC clock */
	RCC->APB2ENR |= (1UL << 9);
	
	/* Calibration Setup */
	if((ADC1->CR & ADC_CR_ADEN) != 0){
		ADC1->CR &= (uint32_t)(~ADC_CR_ADEN);
	}
	
	ADC1->CR |= ADC_CR_ADCAL;
	while((ADC1->ISR & ADC_ISR_EOCAL) == 0);
	ADC1->ISR |= ADC_ISR_EOCAL;
	
	/* Enable ADC */
	ADC1->CR |= (1UL << 0);
	
	/* Wait for ISR bit to set */
	while((ADC1->ISR & 1) == 0);
}

/**
  \fn					int ADC_Pin(int pin)
  \brief			Creates a 12-bit A2D read based on pin choice, in continuous mode
	\param			int pin: The pin that you would like to use for A2D
	\returns		int ADC_Conversion: Returns a value 0:4096
*/

int ADC_Pin(int pin){
	
	/* Local Variable */
	int ADC_Conversion = 0;
	
	/* Select which pin will be ADC 0...18 */
	ADC1->CHSELR |= (1UL << pin);
	
	/* Enable Continuous mode */
	ADC1->CFGR1 |= (1UL << 13);
	
	/* Right Aligned data in DR register */
	ADC1->CFGR1 |= (0UL << 5);
	
	/* 12-Bit Resolution */
	ADC1->CFGR1 |= (0Ul << 3);
	
	/* Start Conversion */
	ADC1->CR |= (1UL << 2);
	
	/* Grab the last 12-Bit Data part */
	ADC_Conversion = ADC1->DR & 0x00000FFF;
	
	return(ADC_Conversion);
}

/**
  \fn					Voltage_Conversion(int ADC_Reading)
  \brief			Convert the ADC value into Voltage
	\param			int ADC_Reading: The converted value to assess
	\returns		float voltage: actual voltage
*/

float Voltage_Conversion(int ADC_Reading){

	/* Local Variables */
	float voltage = 0.0;
	float voltage_Per_Division = 3.3/4096;		//Supply voltage is known as 3.3.
	
	/* Calculate Voltage */
	voltage = ADC_Reading*voltage_Per_Division;
	return(voltage);
}

/**
  \fn          Unitialize_ADC(void)
  \brief       Sequence to disable the ADC
*/

void Unitialize_ADC(void){
	
	/* Check that no conversion is ongoing */
	if((ADC1->CR & ADC_CR_ADSTART) != 0){
		//Stop on-going conversion
		ADC1->CR |= ADC_CR_ADSTP;
	}
	
	while((ADC1->CR & ADC_CR_ADSTP) != 0){
		// Wait until conversion is stopped
	}
	
	/* Disable the ADC */
	ADC1->CR |= ADC_CR_ADDIS; 
	while((ADC1->CR & ADC_CR_ADEN) != 0){
		// Wait until ADC is disabled
	}
}
