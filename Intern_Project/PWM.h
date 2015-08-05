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
 
#ifndef PWM_H
#define PWM_H

extern void PWM(TIM_TypeDef* TIMx, int Pulse_Duration,GPIO_TypeDef* GPIOx, int Pin);
extern int Servo_Position(int Degrees);
extern void Release_Servo(TIM_TypeDef* TIMx, GPIO_TypeDef* GPIOx, int Pin);

#endif

