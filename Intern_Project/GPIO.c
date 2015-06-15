#include "stm32l0xx.h"                  // Device header
#include "GPIO.h"

#define green_LED  5										//LD2 on nucleo board
#define blue_Button		13								//B1 User button

/*--------------------------------------------------Global Variables---------------------------------------------*/


/**
  \fn 			void Initialize_Port_A(int pin, unsigned long mode, unsigned long output_Type, unsigned long output_Speed, unsigned long pupd)
  \brief		Initialize pins on PORTA
	\param		int pin: The number of the pin you want to set ie PORTC pin 13 would be 13
	\param 		unsigned long mode:
							*Set Mode
							*00 : input mode
							*01 : General purpose output mode
							*10 : Alternate function mode
							*11 : Analog Mode(reset state)
	\param		unsigned long output_Type:
							*Set output type 
							* 0 : output push/pull (reset state)
							* 1 : output open drain
	\param		unsigned long output_Speed: 
							*Set GPIOx_OSPEEDR
							* 00 : very low speed
							* 01 : Low speed
							* 10 : medium speed
							* 11 : High speed
	\param		unsigned long pupd:
							*Set GPIOx_PUPDR
							* 00 : No pull up, pull down
							* 01 : Pull up
							* 10 : Pull down
							* 11 : Reserved
*/

void Initialize_Port_A(int pin, unsigned long mode, unsigned long output_Type, unsigned long output_Speed, unsigned long pupd){
	
	/*---------------GPIO Clock Init----------------------------------------------*/
	RCC->IOPENR |=  (1UL << 0);															// Enable GPIOA clock
	/*----------------------------------------------------------------------------*/
	
	/*---------------GPIO Mode Init-----------------------------------------------*/
	GPIOA->MODER   &= ~((3ul << 2*pin));		// write 00 to pin location
  GPIOA->MODER   |=  ((mode << 2*pin));		// Choose mode
	/*----------------------------------------------------------------------------*/
	
	/*---------------Output Type Init---------------------------------------------*/
  GPIOA->OTYPER  &= ~((~(output_Type) <<   pin));
	/*----------------------------------------------------------------------------*/
	
	/*--------------------GPIO Speed Init-----------------------------------------*/
  GPIOA->OSPEEDR |=  ((output_Speed << 2*pin));
	/*----------------------------------------------------------------------------*/
	
	/*---------------------GPIO PULLUP/PULLDOWN Init------------------------------*/
  GPIOA->PUPDR   |= (pupd << 2*pin);
	/*----------------------------------------------------------------------------*/
}

/**
  \fn          void Unitialize_Port_A(void)
  \brief       Uninitialize PortA
*/

void Unitialize_Port_A(void){
	GPIOA->OTYPER = 0X00000000;											//Reset for portA
	GPIOA->MODER = 0xEBFFFCFF;											//Reset for portA
	GPIOA->PUPDR = 0X24000000;											//Reset for portA
	GPIOA->OSPEEDR = 0x0C000000;									//Reset for portA
}

/**
  \fn 			void Initialize_Port_C(int pin, unsigned long mode, unsigned long output_Type, unsigned long output_Speed, unsigned long pupd)
  \brief		Initialize pins on PORTC
	\param		int pin: The number of the pin you want to set ie PORTC pin 13 would be 13
	\param 		unsigned long mode:
							*Set Mode
							*00 : input mode
							*01 : General purpose output mode
							*10 : Alternate function mode
							*11 : Analog Mode(reset state)
	\param		unsigned long output_Type:
							*Set output type 
							* 0 : output push/pull (reset state)
							* 1 : output open drain
	\param		unsigned long output_Speed: 
							*Set GPIOx_OSPEEDR
							* 00 : very low speed
							* 01 : Low speed
							* 10 : medium speed
							* 11 : High speed
	\param		unsigned long pupd:
							*Set GPIOx_PUPDR
							* 00 : No pull up, pull down
							* 01 : Pull up
							* 10 : Pull down
							* 11 : Reserved
*/

void Initialize_Port_C(int pin, unsigned long mode, unsigned long output_Type, unsigned long output_Speed, unsigned long pupd){

	/*---------------GPIO Clock Init----------------------------------------------*/
	RCC->IOPENR |=  (1ul << 2);                   	// Enable GPIOC clock
	/*----------------------------------------------------------------------------*/
	
	/*---------------GPIO Mode Init-----------------------------------------------*/
	GPIOC->MODER &= (mode << 2*pin);
	/*----------------------------------------------------------------------------*/
	
	/*---------------Output Type Init---------------------------------------------*/
  GPIOC->OTYPER  &= ~((~(output_Type) <<   pin));
	/*----------------------------------------------------------------------------*/
	
	/*--------------------GPIO Speed Init-----------------------------------------*/
  GPIOC->OSPEEDR |=  ((output_Speed << 2*pin));
	/*----------------------------------------------------------------------------*/
	
	/*---------------------GPIO PULLUP/PULLDOWN Init------------------------------*/
  GPIOC->PUPDR   |= (pupd << 2*pin);
	/*----------------------------------------------------------------------------*/
}

/**
  \fn          void Unitialize_Port_C(void)
  \brief       Uninitialize User Button
*/

void Unitialize_Port_C(void){
	GPIOC->OTYPER = 0X00000000;											//Reset for portC
	GPIOC->MODER = 0XFFFFFFFF;											//Reset for portC
	GPIOC->PUPDR = 0X00000000;											//Reset for portC
	GPIOC->OSPEEDR = 0x00000000; 										//Reset for portC
}

/**
  \fn          void Button_Initialize (void)
  \brief       Initialize User Button
*/

void Button_Initialize (void) {
	
	Initialize_Port_C(blue_Button,0,0,1,0);
	
}

/**
  \fn          void LED_Init(void)
  \brief       Initialize LD2
*/

void LED_Init(void){
	Initialize_Port_A(green_LED,1,0,3,0);
}

/**
  \fn          void LED_On(void)
  \brief       Turn LD2 on
*/

void LED_On(void){
	//Local Variables
	const unsigned long gpio_Pin = {1UL << green_LED};			//calculate position of pin
	
	GPIOA->BSRR |= (gpio_Pin);											// Set PORTA-5 [Green LED]
	//GPIOA->ODR = 0x20;														// Same as using the BSRR register
}

/**
  \fn          void LED_Off(void)
  \brief       Turn LD2 off
*/

void LED_Off(void){
	//Local Variables
	const unsigned long gpio_Pin = {1UL << green_LED};			//calculate position of pin
	
	GPIOA->BSRR |= (gpio_Pin << 16);              	// Turn specified LED off
}

/**
  \fn          int Button_Get_State (void)
  \brief       Get buttons state
  \returns     Buttons state
*/
int Button_Get_State (void) {

  int val = 0;

  if ((GPIOC->IDR & (1UL << blue_Button)) == 0) {
    /* USER button */
    val |= 1;
  }

  return (val);
}
