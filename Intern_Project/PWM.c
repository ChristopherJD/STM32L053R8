/*------------------------------------------------------------------------------------------------------
 * Name:    PWM.c
 * Purpose: Configures pulse width modulation
 * Date: 		8/4/15
 * Author:	Christopher Jordan - Denny
 *------------------------------------------------------------------------------------------------------
 * Note(s): 
 *----------------------------------------------------------------------------------------------------*/
 /*------------------------------------------------------Include Statements---------------------------*/
 #include "stm32l053xx.h"					//Specific Device header
 #include "GPIO.h"
 /*------------------------------------------------------Functions------------------------------------*/
 
void PWM_Init(TIM_TypeDef* TIMx, int Pulse_Duration,GPIO_TypeDef* GPIOx, int Pin){
 
 /*Initialize GPIO*/
 struct GPIO_Parameters GPIO;
 GPIO.Pin = Pin;
 GPIO.Mode = Alternate_Function;
 GPIO.Speed = High_Speed;
 GPIO_Init(GPIOx,GPIO);
 
 /*Enable TIM clock*/
 if(TIMx == TIM22){
		RCC->APB2ENR |= RCC_APB2ENR_TIM22EN;
 }
 if(TIMx == TIM21){
	 RCC->APB2ENR |= RCC_APB2ENR_TIM21EN;
 }
 
 TIMx->PSC = 0x1F;						//CK_CNT=Fck_psc/(PSC[15:0]+1), so 32MHz clock becomes 1MHz
 TIMx->ARR = 0xFA0;						//Clock is 1MHz so period becomes 4ms
 
 /*Pulse Width Calculation*/
 TIMx->CCR1 = (TIMx->ARR+1)-Pulse_Duration;
 
 /*Select PWM 2 on OC1 and enable preload register*/
 TIMx->CCMR1 |= TIM_CCMR1_OC1M_2|TIM_CCMR1_OC1M_1|TIM_CCMR1_OC1M_0
								|TIM_CCMR1_OC1PE
 #if PULSE_WITHOUT_DELAY > 0
								|TIM_CCMR1_OC1FE
 #endif
 ;
 
 /*Select active high polarity on OC1*/
 TIMx->CCER = TIM_CCER_CC1E;
 
 /*Auto reload preload enable and One pulse mode stops counting at next update event*/
 TIMx->CR1 = TIM_CR1_OPM | TIM_CR1_ARPE;
 TIMx->EGR = 0;
 
 TIMx->CR1 |= TIM_CR1_CEN;
}
 
void Position_180(void){
 PWM_Init(TIM22,1000,GPIOC,6);
}

void Position_90(void){
 PWM_Init(TIM22,1500,GPIOC,6);
}

void Position_0(void){
 PWM_Init(TIM22,2000,GPIOC,6);
}

