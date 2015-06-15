/*----------------------------------------------------------------------------
 * Name:    Serial.c
 * Purpose: Low Level Serial Routines
 * Note(s): possible defines select the used communication interface:
 *                        - USART2 interface  (default)
 *----------------------------------------------------------------------------
 * This file is part of the uVision/ARM development tools.
 * This software may only be used under the terms of a valid, current,
 * end user licence from KEIL for a compatible version of KEIL software
 * development tools. Nothing else gives you the right to use this software.
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2014 Keil - An ARM Company. All rights reserved.
 *----------------------------------------------------------------------------*/

#include "stm32l0xx.h"                  // Device header
#include "Serial.h"

/*----------------------------------------------------------------------------
 Define  USART
 *----------------------------------------------------------------------------*/
#define USARTx  USART2


/*----------------------------------------------------------------------------
 Define  Baudrate setting (BRR) for USART
 *----------------------------------------------------------------------------*/
#define __DIV(__PCLK, __BAUD)       ((__PCLK*25)/(4*__BAUD))
#define __DIVMANT(__PCLK, __BAUD)   (__DIV(__PCLK, __BAUD)/100)
#define __DIVFRAQ(__PCLK, __BAUD)   (((__DIV(__PCLK, __BAUD) - (__DIVMANT(__PCLK, __BAUD) * 100)) * 16 + 50) / 100)
#define __USART_BRR(__PCLK, __BAUD) ((__DIVMANT(__PCLK, __BAUD) << 4)|(__DIVFRAQ(__PCLK, __BAUD) & 0x0F))


/*----------------------------------------------------------------------------
  Initialize UART pins, Baudrate
 *----------------------------------------------------------------------------*/
void SER_Initialize (void) {

  RCC->IOPENR   |=   ( 1ul <<  0);         /* Enable GPIOA clock              */
  RCC->APB1ENR  |=   ( 1ul << 17);         /* Enable USART#2 clock            */

  /* Configure PA3 to USART2_RX, PA2 to USART2_TX */
  GPIOA->AFR[0] &= ~((15ul << 4* 3) | (15ul << 4* 2) );
  GPIOA->AFR[0] |=  (( 4ul << 4* 3) | ( 4ul << 4* 2) );
  GPIOA->MODER  &= ~(( 3ul << 2* 3) | ( 3ul << 2* 2) );
  GPIOA->MODER  |=  (( 2ul << 2* 3) | ( 2ul << 2* 2) );

  USARTx->BRR  = __USART_BRR(32000000ul, 9600ul);  /* 9600 baud @ 32MHz   */
  USARTx->CR3    = 0x0000;                 /* no flow control                 */
  USARTx->CR2    = 0x0000;                 /* 1 stop bit                      */
  USARTx->CR1    = ((   1ul <<  2) |       /* enable RX                       */
                    (   1ul <<  3) |       /* enable TX                       */
                    (   0ul << 12) |       /* 8 data bits                     */
                    (   0ul << 28) |       /* 8 data bits                     */
                    (   1ul <<  0) );      /* enable USART                    */
}


/*----------------------------------------------------------------------------
  Write character to Serial Port
 *----------------------------------------------------------------------------*/
int SER_PutChar (int ch) {

  while (!(USARTx->ISR & 0x0080));
  USARTx->TDR = (ch & 0xFF);

  return (ch);
}

/*----------------------------------------------------------------------------
  Read character from Serial Port
 *----------------------------------------------------------------------------*/
int SER_GetChar (void) {

  if (USARTx->ISR & 0x0020)
    return (USARTx->RDR);

  return (-1);
}

