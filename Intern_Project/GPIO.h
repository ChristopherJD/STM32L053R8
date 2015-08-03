/*------------------------------------------------------------------------------------------------------
 * Name:    GPIO.h
 * Purpose: Initializes GPIO and has LED and Button functions for the green LED and the 
						User button
 * Date: 		7/14/15
 * Author:	Christopher Jordan - Denny
 *------------------------------------------------------------------------------------------------------
 * Note(s):
 *----------------------------------------------------------------------------------------------------*/

/*-----------------------------------Include Statements-----------------------------------------------*/
#include "stm32l053xx.h"

#ifndef GPIO_H
#define GPIO_H

struct GPIO_Parameters
{
	int Pin;
	int Mode;
	int Speed;
	int OType;
	int PuPd;
};

typedef enum Mode_Choices
{
	Input									= 0,
	Output								= 1,
	Alternate_Function		= 2,
	Analog_Mode						= 3
}Mode_Choices;

typedef enum OType_Choices
{
	Push_Pull		= 0,
	Open_Drain	= 1
}OType_Choices;

typedef enum Speed_Choices
{
	Very_Low_Speed		= 0,
	Low_Speed					= 1,
	Medium_Speed			= 2,
	High_Speed				= 3
}Speed_Choices;

typedef enum PuPd_Choices
{
	No_PuPd			= 0,
	Pull_Up			= 1,
	Pull_Down		= 2,
}PuPd_Choices;

extern void GPIO_Init(GPIO_TypeDef* GPIOx, struct GPIO_Parameters GPIO);
extern void GPIO_Uninit(GPIO_TypeDef* GPIOx);
extern void Button_Initialize(void);
extern void GPIO_Output_Init(GPIO_TypeDef* GPIOx,int LED);
extern void GPIO_On(GPIO_TypeDef* GPIOx,int LED);
extern void GPIO_Off(GPIO_TypeDef* GPIOx,int LED);
extern int Button_Get_State(void);

#endif
