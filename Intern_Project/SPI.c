/*-------------------------------------------------------------------------------------------------------------------
 * Name:    SPI.c
 * Purpose: SPI Initialization
 * Date: 		6/18/15
 * Author:	Christopher Jordan - Denny
 *-------------------------------------------------------------------------------------------------------------------
 * Note(s): 
 *-------------------------------------------------------------------------------------------------------------------*/
 
 /*---------------------------------------------Include Statements---------------------------------------------------*/
 #include "stm32l053xx.h"				//Specific Device Header
 #include "SPI.h"								//SPI Driver header
 
 void SPI_Master_Init(SPI_TypeDef* SPIx){
	 
	 if(SPIx == SPI1){
		 
		 /* Enable GPIO clock */
		 
		 /* Enable SPI1 clock */
		 RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
		 
	 }
	 
	 /* SPI Configuration */
	 SPIx->CR1 = SPI_CR1_MSTR | SPI_CR1_BR;
	 SPIx->CR2 = SPI_CR2_SSOE | SPI_CR2_RXNEIE;
	 
	 /* Start SPI */
	 SPIx->CR1 = SPI_CR1_SPE;
	 
 }

 
 