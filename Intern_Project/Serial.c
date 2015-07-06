
#include "stm32l0xx.h"                  // Device header
#include "Serial.h"

#define USARTx  USART2

/* Alternate USARTx_BRR calculation */
//#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
//#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
//#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
//#define __USART_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))


void SER_Initialize (void) {

  RCC->IOPENR   |=   ( 1ul <<  0);         /* Enable GPIOA clock              */
  RCC->APB1ENR  |=   ( 1ul << 17);         /* Enable USART#2 clock            */

  /* Configure PA3 to USART2_RX, PA2 to USART2_TX */
  GPIOA->AFR[0] &= ~((15ul << 4* 3) | (15ul << 4* 2) );
  GPIOA->AFR[0] |=  (( 4ul << 4* 3) | ( 4ul << 4* 2) );
  GPIOA->MODER  &= ~(( 3ul << 2* 3) | ( 3ul << 2* 2) );
  GPIOA->MODER  |=  (( 2ul << 2* 3) | ( 2ul << 2* 2) );

  USARTx->BRR  = 0xD05;  										/* 9600 baud @ 32MHz   */
  USARTx->CR3    = 0x0000;                 /* no flow control                 */
  USARTx->CR2    = 0x0000;                 /* 1 stop bit                      */
  USARTx->CR1    = ((   1ul <<  2) |       /* enable RX                       */
                    (   1ul <<  3) |       /* enable TX                       */
                    (   0ul << 12) |       /* 8 data bits                     */
                    (   0ul << 28) |       /* 8 data bits                     */
                    (   1ul <<  0) );      /* enable USART                    */
}

char SER_PutChar (char ch) {

	//Wait for buffer to be empty
  while ((USARTx->ISR & USART_ISR_TXE) == 0){
			//Nop
	}
	
	//Send character
  USARTx->TDR = (ch);

  return (ch);
}

/*----------------------------------------------------------------------------
  Read character from Serial Port
 *----------------------------------------------------------------------------*/
int SER_GetChar (void) {

  if (USARTx->ISR & USART_ISR_RXNE)
    return (USARTx->RDR);

  return (-1);
}

