/*------------------------------------------------------------------------------------------------------
 * Name:    GPIO.c
 * Purpose: Initializes GPIO and has LED and Button functions for the green LED and the 
						User button
 * Date: 		7/14/15
 * Author:	Christopher Jordan - Denny
 *------------------------------------------------------------------------------------------------------
 * Note(s):
 *----------------------------------------------------------------------------------------------------*/

/*---------------------------------------------Include Statements-------------------------------------*/
#include "stm32l053xx.h"									// Specific Device header
#include "GPIO.h"
/*---------------------------------------------Definitions--------------------------------------------*/
#define Green_LED  		5										//LD2 on nucleo board
#define Blue_Button		13									//B1 User button
/*---------------------------------------------Functions----------------------------------------------*/

/**
  \fn 			void GPIO_Init(GPIO_TypeDef* GPIOx, struct GPIO_Parameters GPIO)
  \brief		Initialize GPIO
	\param		GPIO_TypeDef* GPIOx: Which port to initialize, i.e. GPIOA,GPIOB
	\param		struct GPIO_Parameters GPIO: Structure containing all GPIO parameters:
						* Pin
						*	Mode
						*	Output Type
						* Output Speed
						* Pull up / Pull down
*/

void GPIO_Init(GPIO_TypeDef* GPIOx, struct GPIO_Parameters GPIO){
	
	/* Enable GPIO Clock depending on port */
	if(GPIOx == GPIOA) RCC->IOPENR |= RCC_IOPENR_GPIOAEN;		// Enable GPIOA clock
	if(GPIOx == GPIOB) RCC->IOPENR |= RCC_IOPENR_GPIOBEN;		// Enable GPIOB clock
	if(GPIOx == GPIOC) RCC->IOPENR |= RCC_IOPENR_GPIOCEN;		// Enable GPIOC clock
	if(GPIOx == GPIOD) RCC->IOPENR |= RCC_IOPENR_GPIODEN;		// Enbale GPIOD clock
	
	/* GPIO Mode Init */
	GPIOx->MODER   &= ~((3ul << 2*GPIO.Pin));					// write 00 to pin location
  GPIOx->MODER   |=  ((GPIO.Mode << 2*GPIO.Pin));		// Choose mode
	
	/* Output Type Init */
  GPIOx->OTYPER  &= ~((~(GPIO.OType) << GPIO.Pin));
	
	/* GPIO Speed Init */
  GPIOx->OSPEEDR |=  ((GPIO.Speed << 2*GPIO.Pin));
	
	/* GPIO PULLUP/PULLDOWN Init */
  GPIOx->PUPDR   |= (GPIO.PuPd << 2*GPIO.Pin);
}

/**
  \fn					void GPIO_Uninit(GPIO_TypeDef* GPIOx)
  \brief			Reset the given port
	\param			GPIO_TypeDef* GPIOx: Which port to reset
*/

void GPIO_Uninit(GPIO_TypeDef* GPIOx){
	
	/* GPIOA has different reset parameters */
	if(GPIOx == GPIOA){
		GPIOA->OTYPER = 0X00000000;											//Reset for portA
		GPIOA->MODER = 0xEBFFFCFF;											//Reset for portA
		GPIOA->PUPDR = 0X24000000;											//Reset for portA
		GPIOA->OSPEEDR = 0x0C000000;										//Reset for portA
	}
	
	/* Normal Reset States */
	else{
		GPIOx->OTYPER = 0X00000000;											//Reset for port
		GPIOx->MODER = 0XFFFFFFFF;											//Reset for port
		GPIOx->PUPDR = 0X00000000;											//Reset for port
		GPIOx->OSPEEDR = 0x00000000; 										//Reset for port
	}
}

/**
  \fn          void Button_Initialize (void)
  \brief       Initialize User Button
*/

void Button_Initialize(void){
	
	/* Set port parameters */
	struct GPIO_Parameters GPIO;
	GPIO.Pin	= Blue_Button;
	GPIO.Mode = Input;
	GPIO.OType = Push_Pull;
	GPIO.PuPd = No_PuPd;
	GPIO.Speed = Low_Speed;
	
	/* Initialize button */
	GPIO_Init(GPIOC,GPIO);
}

/**
  \fn          void LED_Init(void)
  \brief       Initialize LD2
*/

void LED_Init(void){
	
	/* Set port parameters */
	struct GPIO_Parameters GPIO;
	GPIO.Pin = Green_LED;
	GPIO.Mode = Output;
	GPIO.OType = Push_Pull;
	GPIO.PuPd = No_PuPd;
	GPIO.Speed = High_Speed;
	
	/* Initialize the LED */
	GPIO_Init(GPIOA,GPIO);
}

/**
  \fn          void LED_On(void)
  \brief       Turn LD2 on
*/

void LED_On(void){
	/* Local Variables */
	const unsigned long gpio_Pin = {1UL << Green_LED};
	
	/* Turn on GPIOA-5 */
	GPIOA->BSRR |= (gpio_Pin);
	//GPIOA->ODR = 0x20;				// Same as using the BSRR register
}

/**
  \fn          void LED_Off(void)
  \brief       Turn LD2 off
*/

void LED_Off(void){
	/* Local Variables */
	const unsigned long gpio_Pin = {1UL << Green_LED};
	
	/* Turn the LED off */
	GPIOA->BSRR |= (gpio_Pin << 16);
}

/**
  \fn          int Button_Get_State (void)
  \brief       Get buttons state
  \returns     Buttons state
*/
int Button_Get_State(void){

  int val = 0;

  if ((GPIOC->IDR & (1UL << Blue_Button)) == 0) {
    /* USER button */
    val |= 1;
  }

  return (val);
}
