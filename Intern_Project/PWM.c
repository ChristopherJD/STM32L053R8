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
 
void PWM(TIM_TypeDef* TIMx, int Pulse_Duration,GPIO_TypeDef* GPIOx, int Pin){
 
 /*Initialize GPIO*/
 struct GPIO_Parameters GPIO;
 GPIO.Pin 	= Pin;
 GPIO.Mode 	= Alternate_Function;
 GPIO.Speed = High_Speed;
 GPIO.PuPd 	= Pull_Down;
 GPIO_Init(GPIOx,GPIO);
 
 /*Enable TIM clock*/
 if(TIMx == TIM22){
		RCC->APB2ENR |= RCC_APB2ENR_TIM22EN;
 }
 if(TIMx == TIM21){
	 RCC->APB2ENR |= RCC_APB2ENR_TIM21EN;
 }
 
 TIMx->PSC = 31;							//CK_CNT=Fck_psc/(PSC[15:0]+1), so 32MHz clock becomes 1MHz
 TIMx->ARR = 20000;						//Clock is 1MHz so period becomes 20ms
 
 /*Pulse Width Calculation*/
 TIMx->CCR1 = (TIMx->ARR)-Pulse_Duration;
 
 /*Select PWM 2 on OC1 and enable preload register*/
 TIMx->CCMR1 |= TIM_CCMR1_OC1M_2|TIM_CCMR1_OC1M_1|TIM_CCMR1_OC1M_0
								|TIM_CCMR1_OC1PE
 #if PULSE_WITHOUT_DELAY > 0
								|TIM_CCMR1_OC1FE
 #endif
 ;
 
 /*Select active high polarity on OC1*/
 TIMx->CCER |= TIM_CCER_CC1E;
 
 /*Enable PWM*/
 TIMx->CR1 = TIM_CR1_CEN;
 TIMx->EGR = TIM_EGR_UG;
}

int Servo_Position(int Degrees){
	
	/*Local Variables*/
	int Position = 0;
	
	/*Degrees to PWM Pulse duration*/
	Position = (Degrees * 10) + 500;
	PWM(TIM22,Position,GPIOC,6);
	
	return(Degrees);
}

void Release_Servo(TIM_TypeDef* TIMx, GPIO_TypeDef* GPIOx, int Pin){
	
	/*Initialize GPIO*/
	struct GPIO_Parameters GPIO;
	GPIO.Pin 	= Pin;
	GPIO.Mode 	= Output;
	GPIO.Speed = High_Speed;
	GPIO.PuPd 	= Pull_Down;
	GPIO_Init(GPIOx,GPIO);
	
	TIMx->CR1 &= ~TIM_CR1_CEN;
	
	 GPIO_Off(GPIOx,6);
}
