/*------------------------------------------------------------------------------------------------------
 * Name:    Timing.c
 * Purpose: Sets up the system clock and also has a delay function
 * Date: 		7/14/15
 * Author:	Christopher Jordan - Denny
 *------------------------------------------------------------------------------------------------------
 * Note(s):
 *----------------------------------------------------------------------------------------------------*/

/*---------------------------------------Include Statements-------------------------------------------*/
#include "stm32l0xx.h"                  // Device header
#include "Timing.h"
/*---------------------------------------Global Variables---------------------------------------------*/
volatile unsigned int msTicks;															// counts 1ms timeTicks
/*---------------------------------------Functions----------------------------------------------------*/

/**
  \fn          void SysTick_Handler(void)
	\brief       SysTick handler that increases msTicks 
*/

void SysTick_Handler(void){
  msTicks++;
}

/**
  \fn          void SystemCoreClockInit(void)
	\brief       SystemCoreClockConfigure: Uses HSI clock
*/
void SystemCoreClockInit(void){

  RCC->CR |= ((uint32_t)RCC_CR_HSION);                     // Enable HSI
  while ((RCC->CR & RCC_CR_HSIRDY) == 0);                  // Wait for HSI Ready

  RCC->CFGR = RCC_CFGR_SW_HSI;                             // HSI is system clock
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);  // Wait for HSI used as system clock

  // PLL configuration: PLLCLK = (HSI * 6)/3 = 32 MHz
  RCC->CFGR &= ~(RCC_CFGR_PLLSRC |
                 RCC_CFGR_PLLMUL |
                 RCC_CFGR_PLLDIV  );
  RCC->CFGR |=  (RCC_CFGR_PLLSRC_HSI |
                 RCC_CFGR_PLLMUL4    |
                 RCC_CFGR_PLLDIV2     );

  FLASH->ACR |= FLASH_ACR_PRFTEN;                          // Enable Prefetch Buffer
  FLASH->ACR |= FLASH_ACR_LATENCY;                         // Flash 1 wait state

  RCC->APB1ENR |= RCC_APB1ENR_PWREN;                       // Enable the PWR APB1 Clock
  PWR->CR = PWR_CR_VOS_0;                                  // Select the Voltage Range 1 (1.8V)
  while((PWR->CSR & PWR_CSR_VOSF) != 0);                   // Wait for Voltage Regulator Ready

  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;                         // HCLK = SYSCLK
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;                        // PCLK1 = HCLK
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;                        // PCLK2 = HCLK

  RCC->CR &= ~RCC_CR_PLLON;                                // Disable PLL

  RCC->CR |= RCC_CR_PLLON;                                 // Enable PLL
  while((RCC->CR & RCC_CR_PLLRDY) == 0) __NOP();           // Wait till PLL is ready

  RCC->CFGR &= ~RCC_CFGR_SW;                               // Select PLL as system clock source
  RCC->CFGR |=  RCC_CFGR_SW_PLL;
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);  // Wait till PLL is system clock src
}

/**
  \fn          void Delay (unsigned int dlyTicks)
  \brief       Delay a number of SysTicks
*/

void Delay (unsigned int dlyTicks){
  unsigned int curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) { __NOP(); }
}
