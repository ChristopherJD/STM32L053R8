#include "stm32l053xx.h"					//Specific Device header

void TIM2_IRQHandler(void){
	
}

void Timer2_Init(void){
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	
	TIM2->PSC = 31999;								//CK_CNT=Fck_psc/(PSC[15:0]+1), so 32MHz clock becomes 1kHz
  TIM2->ARR = 1000;									//Clock is 1kHz so period becomes 1s
}

