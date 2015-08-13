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
#include "PWM.h"
/*---------------------------------------Global Variables---------------------------------------------*/
volatile unsigned int msTicks;															// counts 1ms timeTicks
unsigned int Start_Timer = 0;														// Start Timer is false
unsigned int Current_Ticks = 0;															// Current Tick
unsigned int Ticks = 0;																			// Just another msTick
/*---------------------------------------Functions----------------------------------------------------*/

/**
  \fn          void SysTick_Handler(void)
	\brief       SysTick handler that increases msTicks 
*/

void SysTick_Handler(void){
  msTicks++;
	
	if(Start_Timer == 1){
		Ticks++;
		if(Ticks > 15000){
			Servo_Position(170);
			Ticks = 0;
			Start_Timer = 0;
		}
	}
}

/**
  \fn          void SystemCoreClockInit(void)
	\brief       SystemCoreClockConfigure: Uses HSI clock
*/
void SystemCoreClockInit(void){

  /* Enable HSI */
	RCC->CR |= ((uint32_t)RCC_CR_HSION);
	
	/* Wait for HSI to be ready */
  while ((RCC->CR & RCC_CR_HSIRDY) == 0){
		// Nop
	}

	/* Set HSI as the System Clock */
  RCC->CFGR = RCC_CFGR_SW_HSI;
	
	/* Wait for HSI to be used for teh system clock */
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI){
		// Nop
	}
	
	FLASH->ACR |= FLASH_ACR_PRFTEN;                          // Enable Prefetch Buffer
  FLASH->ACR |= FLASH_ACR_LATENCY;                         // Flash 1 wait state

  RCC->APB1ENR |= RCC_APB1ENR_PWREN;                       // Enable the PWR APB1 Clock
  PWR->CR = PWR_CR_VOS_0;                                  // Select the Voltage Range 1 (1.8V)
  while((PWR->CSR & PWR_CSR_VOSF) != 0);                   // Wait for Voltage Regulator Ready

  /* PLLCLK = (HSI * 4)/2 = 32 MHz */
  RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMUL | RCC_CFGR_PLLDIV);				/* Clear */
  RCC->CFGR |=  (RCC_CFGR_PLLSRC_HSI | RCC_CFGR_PLLMUL4 | RCC_CFGR_PLLDIV2);	/* Set   */
	
	/* Peripheral Clock divisors */
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;                         // HCLK = SYSCLK
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;                        // PCLK1 = HCLK
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;                        // PCLK2 = HCLK

	/* Enable PLL */
  RCC->CR &= ~RCC_CR_PLLON;		/* Disable PLL */
  RCC->CR |= RCC_CR_PLLON;		/* Enable PLL	 */
	
	/* Wait until the PLL is ready */
  while((RCC->CR & RCC_CR_PLLRDY) == 0){
		//Nop
	}
	
	/* Select PLL as system Clock */
  RCC->CFGR &= ~RCC_CFGR_SW;			/* Clear */
  RCC->CFGR |=  RCC_CFGR_SW_PLL;	/* Set   */
	
	/* Wait for PLL to become system core clock */
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL){
		//Nop
	}
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

void Start_15s_Timer(void){

	Start_Timer = 1;
	
}
