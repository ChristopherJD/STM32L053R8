/*-------------------------------------------------------------------------------------------------------------------
 * Name:    XBeePro24.c
 * Purpose: Initialize LPUSART, XBee, and send all data acquired
 * Date: 		6/18/15
 * Author:	Christopher Jordan - Denny
 *-------------------------------------------------------------------------------------------------------------------
 * Note(s): 
 *-------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------Include Statements-------------------------------------------------*/
#include "stm32l053xx.h"			//Specific Device Header
#include "string.h"						//String functions
/*---------------------------------Baud Rate Calculations------------------------------------------------------------*/
#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
#define __USART_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))
/*--------------------------------Functions---------------------------------------------------------------------------*/
void RNG_LPUART1_IRQHandler(void){
	if(LPUART1->ISR & USART_ISR_RXNE){
		//Put implementations here
	}
}

void LPUART_Init(void){
	
	RCC->IOPENR   |=   RCC_IOPENR_GPIOBEN;			/* Enable GPIOB clock */
	RCC->APB1ENR  |=   RCC_APB1ENR_LPUART1EN;   /* Enable LP USART#1 clock */
	
	//interrupt init
	NVIC_EnableIRQ(RNG_LPUART1_IRQn);
	NVIC_SetPriority(RNG_LPUART1_IRQn,1);
	
	//Configure PB10 to LPUSART1_TX, PB11 to LPUSART1_RX
  GPIOB->AFR[1] &= ~((15ul << 4* 2) | (15ul << 4* 3) );			/* Set to 0 */
  GPIOB->AFR[1] |=  (( 4ul << 4* 2) | ( 4ul << 4* 3) );			/* Set to alternate function 4 */
  GPIOB->MODER  &= ~(( 3ul << 2* 10) | ( 3ul << 2* 11) );		/* Set to 0 */
  GPIOB->MODER  |=  (( 2ul << 2* 10) | ( 2ul << 2* 11) );		/* Set to alternate function mode */
	
	LPUART1->BRR  = __USART_BRR(32000000ul, 9600ul);  					/* 9600 baud @ 32MHz   */
  LPUART1->CR3    = 0x0000;																	/* no flow control */
  LPUART1->CR1    = ((USART_CR1_RE) |												/* enable RX  */
                    (USART_CR1_TE) |												/* enable TX  */
                    (   0ul << 12) |												/* 8 data bits */
                    (   0ul << 28) |												/* 8 data bits */
                    (USART_CR1_UE) |      									/* enable USART */
										(USART_CR1_RXNEIE));										/* Enable Interrupt */
										
}

char LPUART1_PutChar(char ch) {

	//Wait for buffer to be empty
  while ((USART1->ISR & USART_ISR_TXE) == 0){
			//Nop
	}
	
	//Send character
  USART1->TDR = (ch);

  return (ch);
}

void LPUART1_Send(char c[]){
	
	int String_Length = strlen(c);
	int Counter = 0;
	
	while(Counter < String_Length){
		LPUART1_PutChar(c[Counter]);
		Counter++;
	}
}
