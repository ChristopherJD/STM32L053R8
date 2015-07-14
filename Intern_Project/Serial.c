/*------------------------------------------------------------------------------------------------------
 * Name:    Serial.c
 * Purpose: Used to communicat with your computer using USART2
 * Date: 		7/14/15
 * Author:	Christopher Jordan - Denny
 *------------------------------------------------------------------------------------------------------
 * Note(s):	SER_PutChar are used to redefine printf function
 *----------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------Include Statements---------------------------------*/
#include "stm32l0xx.h"                  // Specific Device header
#include "Serial.h"
/*-------------------------------------------------Functions------------------------------------------*/

/**
  \fn          void SER_Initialize (void)
  \brief       Initializes USART2 to be used with the serial monitor
*/
	
void SER_Initialize (void){

  RCC->IOPENR   |=   ( 1ul <<  0);         /* Enable GPIOA clock   */
  RCC->APB1ENR  |=   ( 1ul << 17);         /* Enable USART#2 clock */

  /* Configure PA3 to USART2_RX, PA2 to USART2_TX */
  GPIOA->AFR[0] &= ~((15ul << 4* 3) | (15ul << 4* 2) );
  GPIOA->AFR[0] |=  (( 4ul << 4* 3) | ( 4ul << 4* 2) );
  GPIOA->MODER  &= ~(( 3ul << 2* 3) | ( 3ul << 2* 2) );
  GPIOA->MODER  |=  (( 2ul << 2* 3) | ( 2ul << 2* 2) );

  USART2->BRR		= 0xD05;  								/* 9600 baud @ 32MHz   */
  USART2->CR3   = 0x0000;                 /* no flow control */
  USART2->CR2   = 0x0000;                 /* 1 stop bit */
  USART2->CR1   = ((   1ul <<  2) |       /* enable RX */
                   (   1ul <<  3) |       /* enable TX */
                   (   0ul << 12) |       /* 8 data bits */
                   (   0ul << 28) |       /* 8 data bits */
                   (   1ul <<  0) );      /* enable USART */
}

/**
  \fn					char SER_PutChar(char ch)
  \brief			Put character to the serial monitor
	\param			char ch: Character to send to the serial monitor
	\returns		char ch: The character that was sent to the serial monitor
*/

char SER_PutChar(char ch){

	//Wait for buffer to be empty
  while ((USART2->ISR & USART_ISR_TXE) == 0){
			//Nop
	}
	
	//Send character
  USART2->TDR = (ch);

  return (ch);
}

/**
  \fn					int SER_GetChar(void)
  \brief			Get character from the serial monitor
	\returns		int USART2->RDR: The value of character from the serial monitor
*/

int SER_GetChar(void){

  if (USART2->ISR & USART_ISR_RXNE)
    return (USART2->RDR);

  return (-1);
}

