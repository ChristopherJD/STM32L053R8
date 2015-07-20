/*------------------------------------------------------------------------------------------------------
 * Name:    I2C.c
 * Purpose: Initializes and reads and writes to I2C
 * Date: 		6/18/15
 * Author:	Christopher Jordan - Denny
 *------------------------------------------------------------------------------------------------------
 * Note(s): The read and write sequence is specific to the ISK01A1, so these functions may not work
						for a different Devices I2C.
 *----------------------------------------------------------------------------------------------------*/

/*-------------------------------------------Include Statements---------------------------------------*/
#include "stm32l053xx.h"                  // Specific Device header
#include "I2C.h"
/*-------------------------------------------Global Variables-----------------------------------------*/
uint32_t I2C1_RX_Data = 0;
/*-------------------------------------------Functions------------------------------------------------*/

/**
  \fn         void I2C_Init(void)
  \brief			I2C initialization
							*PORTB-8: SCL
							*PORTB-9: SDA
							*Digital Noise filter with supression of 1 I2Cclk
							*fast Mode @400kHz with I2CCLK = 16MHz, rise time = 100ns, fall time = 10ns
*/

void I2C_Init(void){
	
	int SCL = 8;							//SCL pin on PORTB alt fnc 4
	int SDA = 9;							//SDA pin on PORTB alt fnc 4
	
	/*Enable Clock for I2C*/
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	
	/* Enable GPIO Clock */
	RCC->IOPENR |=  (1UL << 1);
	
	/* Don't forget to also enable which interrupts you want in CR1 */
	//interrupt init
//	NVIC_EnableIRQ(I2C1_IRQn);
//	NVIC_SetPriority(I2C1_IRQn,0);
	
/** GPIOB Setup
	*	Digital Noise filter with supression of 1 I2Cclk.(1)
	*	Alternate Mode PORTB pin 8 and pin 9.............(2)
	*	Set alternate function 4 for both pin 8 an 9.....(3)
	*/
	I2C1->CR1 |= (1<<8);																								/*(1)*/
	GPIOB->MODER = ~((~GPIOB->MODER) | ((1 << 2*SCL) + (1 << 2*SDA)));	/*(2)*/
	GPIOB->AFR[1] = 0x00000044;																					/*(3)*/
	
/** GPIOB Setup
	*	Standard Mode @100kHz with I2CCLK = 16MHz, rise time = 100ns, fall time = 10ns.(1)
	*	Enable I2C1 peripheral.........................................................(2)
	*/
	I2C1->TIMINGR = (uint32_t)0x00503D5A;	/*(1)*/
	I2C1->CR1 |= I2C_CR1_PE;							/*(2)*/
}

/**
  \fn				void Reset_I2C(void)
  \brief		I2C Reset, clears and then sets I2C_CR1_PE
*/

void Reset_I2C(void){
	int x = 0;		//1 to set bit, 0 to clear bit
	
	I2C1->CR1 ^= (-x ^ I2C1->CR1) & I2C_CR1_PE;		//Clear bit for reset
	I2C1->CR1 |= I2C_CR1_PE;
}

/**
  \fn					uint32_t I2C_Read_Reg(uint32_t Register)
  \brief			Reads a register, the entire sequence to read (at least for HTS221)
	\param			uint32_t Device: The slave address of the device
	\param			uint32_t Register: The Register to read from
	\returns		uint32_t I2C1_RX_Data: The data read
*/

uint32_t I2C_Read_Reg(uint32_t Device,uint32_t Register){
	
	//Reset CR2 Register
	I2C1->CR2 = 0x00000000;
	
	//Check to see if the bus is busy
	while((I2C1->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY);
	
	//Set CR2 for 1-byte transfer for Device
	I2C1->CR2 |=(1UL<<16) | (Device<<1);
	
	//Start communication
	I2C1->CR2 |= I2C_CR2_START;
	
	//Check Tx empty before writing to it
	if((I2C1->ISR & I2C_ISR_TXE) == (I2C_ISR_TXE)){
		I2C1->TXDR = Register;
	}
	
	//Wait for transfer to complete
	while((I2C1->ISR & I2C_ISR_TC) == 0);

	//Clear CR2 for new configuration
	I2C1->CR2 = 0x00000000;
	
	//Set CR2 for 1-byte transfer, in read mode for Device
	I2C1->CR2 |= (1UL<<16) | I2C_CR2_RD_WRN | (Device<<1);
	
	//Start communication
	I2C1->CR2 |= I2C_CR2_START;
	
	//Wait for transfer to complete
	while((I2C1->ISR & I2C_ISR_TC) == 0);
	
	//Send Stop Condition
	I2C1->CR2 |= I2C_CR2_STOP;
	
	//Check to see if the bus is busy
	while((I2C1->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY);

	//Clear Stop bit flag
	I2C1->ICR |= I2C_ICR_STOPCF;
	
	return(I2C1_RX_Data);
}

/**
  \fn					uint32_t I2C_Read_Reg(uint32_t Register)
  \brief			Reads a register, the entire sequence to read (at least for HTS221)
	\param			uint32_t Device: The slave address to written to
	\param			uint32_t Register: The register that you would like to write to
	\param			uint32_t Data: The data that you would like to write to the register
*/

void I2C_Write_Reg(uint32_t Device,uint32_t Register, uint32_t Data){
	
	//Reset CR2 Register
	I2C1->CR2 = 0x00000000;
	
	//Check to see if the bus is busy
	while((I2C1->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY);
	
	//Set CR2 for 2-Byte Transfer, for Device
	I2C1->CR2 |= (2UL<<16) | (Device<<1);
	
	//Start communication
	I2C1->CR2 |= I2C_CR2_START;
	
	//Check Tx empty before writing to it
	if((I2C1->ISR & I2C_ISR_TXE) == (I2C_ISR_TXE)){
		I2C1->TXDR = Register;
	}
	
	//Wait for TX Register to clear
	while((I2C1->ISR & I2C_ISR_TXE) == 0);
	
	//Check Tx empty before writing to it
	if((I2C1->ISR & I2C_ISR_TXE) == I2C_ISR_TXE){
		I2C1->TXDR = Data;
	}
	
	//Wait for transfer to complete
	while((I2C1->ISR & I2C_ISR_TC) == 0);
	
	//Send Stop Condition
	I2C1->CR2 |= I2C_CR2_STOP;	
	
	//Check to see if the bus is busy
	while((I2C1->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY);
	
	//Clear Stop bit flag
	I2C1->ICR |= I2C_ICR_STOPCF;
}
