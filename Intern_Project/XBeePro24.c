/*-------------------------------------------------------------------------------------------------------------------
 * Name:    XBeePro24.c
 * Purpose: Initialize LPUSART, XBee, and sends all data acquired. This is XBEE B in the peer to peer network.
 *					*	Personal Area Network: 	3001
 *					* Destination Address: 		1
 *					* Operating Channel:			C
 *					* My address:							2 
 * Date: 		6/18/15
 * Author:	Christopher Jordan - Denny
 *-------------------------------------------------------------------------------------------------------------------
 * Note(s): 
 *-------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------Include Statements-------------------------------------------------*/
#include "stm32l053xx.h"			//Specific Device Header
#include <stdio.h>						//Standard input output
#include "string.h"						//String functions
#include "Serial.h"						//Printf function
#include "Timing.h"						//Delay function
#include "PWM.h"							//To control the Servo for parachute deployment
#include "XBeePro24.h"
/*---------------------------------XBee Commands----------------------------------------------------------------------*/
/* Prefix(AT) + ASCII Command + Space(Optional) + Parameter(Optional,HEX) + Carridge Return */
#define ENTER_AT_COMMAND_MODE					"+++"					//Enter three plus characters within 1s there is no \r on purpose
#define EXIT_AT_COMMAND_MODE					"ATCN\r"			//Exit AT command mode
#define READ_SERIAL_ADDRESS_HIGH			"ATSH\r"			//Read the high bits of the serial address
#define READ_SERIAL_ADDRESS_LOW				"ATSL\r"			//Read the lower bits of the serial address
#define SET_ATMY											"ATMY 2\r"		//Source address, unique to this device - (Address of XBee B)
#define SET_ATID											"ATID 3001\r"	//Personal Area Network = 3001
#define SET_ATDH											"ATDH 0\r"		//Destination Address high
#define SET_ATDL											"ATDL 1\r"		//Destination Address Low - (The address of XBee A)
#define SET_ATCH											"ATCH c\r"		//Operating Channel Selection
#define SAVE_SETTINGS									"ATWR\r"			//Saves the Settings you have configured
#define READ_ATMY											"ATMY\r"			//Read MY register
#define READ_ATID											"ATID\r"			//Read ID register
#define READ_ATDH											"ATDH\r"			//Read DH register
#define READ_ATDL											"ATDL\r"			//Read DL register
#define READ_ATCH											"ADCH\r"			//Read CH register
/*---------------------------------XBee 900HP Commands----------------------------------------------------------------*/
#define SET_DESTINATION_H							"ATDH 13A200\r"	//Specific Serial Address of Matt's XBee
#define SET_DESTINATION_L							"ATDL 40E35DC2\r"	//Specific Serial Address of Matt's Xbee
#define SET_ATHP											"ATHP 5\r"				//The preamble ID, must be the same for XBee's to communicate
/*---------------------------------More defines-----------------------------------------------------------------------*/
#define TRUE	1;
#define FALSE 0;
#define PCLK	32000000									// Peripheral Clock
#define BAUD	9600											// Baud rate
/*---------------------------------Globals----------------------------------------------------------------------------*/
char 										RX_Data[33] = 				"";				//Rx
uint8_t 								ChIndex =							0;				//Character Index
char 										XBee_Message[33] = 		"";				//Message Recieved by the XBee
static const char				OK[] = 								"OK";			//When data has been written XBee will send an OK
uint8_t									Device_Ack_Flag = 		FALSE;		//XBee OK acknowledge
uint8_t									XBee_Ready_To_Read = 	FALSE;		//XBee data ready
/*--------------------------------Struct Initialize-------------------------------------------------------------------*/
AT_Data AT;
/*--------------------------------Functions---------------------------------------------------------------------------*/

/**
	\fn			void RNG_LPUART1_IRQHandler(void)
	\brief	Global interrupt handler for LPUART, Currently only handles RX
*/

void RNG_LPUART1_IRQHandler(void){
	if((LPUART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE){
		
		/* Read RX Data */
		RX_Data[ChIndex] = LPUART1->RDR;
		
		/* Check for parachute deployment signal */
		if(RX_Data[ChIndex] == '!'){
			Servo_Position(180);
		}
		
		/* Check for end of recieved data */
		if(RX_Data[ChIndex] == '\r'){
			
			/* Compare string to OK to see if Acknowledged */
			if(strncmp(OK,RX_Data,(sizeof(OK)-1)) == 0){
				Device_Ack_Flag = TRUE;
			}
			
			/* Copy XBee message */
			strcpy(XBee_Message,RX_Data);
			
			/* set data ready to read flag */
			XBee_Ready_To_Read = TRUE;
			
			/* Clear RX_Data */
			ChIndex = 0;
			memset(RX_Data,0,sizeof(RX_Data));
			
		}else ChIndex++;
	}
}

/**
	\fn			void LPUART_Init(void)
	\brief	Initializes the Low Powered UART
*/

void LPUART_Init(void){
	
	RCC->IOPENR   |=   RCC_IOPENR_GPIOCEN;			/* Enable GPIOB clock */
	RCC->APB1ENR  |=   RCC_APB1ENR_LPUART1EN;   /* Enable LP USART#1 clock */
	
	//interrupt init
	NVIC_EnableIRQ(RNG_LPUART1_IRQn);
	NVIC_SetPriority(RNG_LPUART1_IRQn,1);
	
	//Configure PC10 to LPUSART1_TX, PC11 to LPUSART1_RX,PC10 and PC11 TX,RX AF0 by default
  GPIOC->MODER  &= ~(( 3ul << 2* 10) | ( 3ul << 2* 11) );		/* Set to 0 */
  GPIOC->MODER  |=  (( 2ul << 2* 10) | ( 2ul << 2* 11) );		/* Set to alternate function mode */
	
	LPUART1->BRR  	= (unsigned long)((256.0f/BAUD)*PCLK); 		/* 9600 baud @ 32MHz */
  LPUART1->CR3    = 0x0000;																	/* no flow control */
	LPUART1->CR2    = 0x0000;																	/* 1 stop bit */
	
	/* 1 stop bit, 8 data bits */
  LPUART1->CR1    = ((USART_CR1_RE) |												/* enable RX  */
                     (USART_CR1_TE) |												/* enable TX  */
                     (USART_CR1_UE) |      									/* enable USART */
										 (USART_CR1_RXNEIE));										/* Enable Interrupt */
										
}

/**
	\fn				char LPUART1_PutChar(char ch)
	\brief		Sends a character to the XBEE
	\returns 	char ch: The character sent to the XBEE
*/

char LPUART1_PutChar(char ch){

	//Wait for buffer to be empty
  while ((LPUART1->ISR & USART_ISR_TXE) == 0){
			//Nop
	}
	
	//Send character
  LPUART1->TDR = (ch);

  return (ch);
}

/**
	\fn				void LPUART1_Send(char c[])
	\brief		Sends a string to the XBEE
*/

void LPUART1_Send(char c[]){
	
	int String_Length = strlen(c);
	int Counter = 0;
	
	while(Counter < String_Length){
		LPUART1_PutChar(c[Counter]);
		Counter++;
	}
}

/**
	\fn				void XBee_900HP_Init(void)
	\brief		Initializes the XBEE
*/
void XBee_900HP_Init(void){
	/* Enter AT command mode */
	Delay(1000);
	LPUART1_Send(ENTER_AT_COMMAND_MODE);
	Wait_For_OK();
	
	/* Serial Address of matt's xbee */
	LPUART1_Send(SET_DESTINATION_H);
	Wait_For_OK();
	
	LPUART1_Send(SET_DESTINATION_L);
	Wait_For_OK();
	
	/* PAN = 3001 */
	LPUART1_Send(SET_ATID);
	Wait_For_OK();
	
	/* HP = 5 */
	LPUART1_Send(SET_ATHP);
	Wait_For_OK();
	
	/* Save the settings */
	LPUART1_Send(SAVE_SETTINGS);
	Wait_For_OK();
	
	/* End AT command mode */
	LPUART1_Send(EXIT_AT_COMMAND_MODE);
	Wait_For_OK();
	
	printf("#####  XBee 	       Initialized  #####\r\n");
}


/**
	\fn				void XBee_Init(void)
	\brief		Initializes the XBEE
*/

void XBee_ProS1_Init(void){
	
	/* Enter AT command mode */
	Delay(1000);
	LPUART1_Send(ENTER_AT_COMMAND_MODE);
	Wait_For_OK();
	
	/* MY address = 2 */
	LPUART1_Send(SET_ATMY);
	Wait_For_OK();
	
	/* PAN = 3001 */
	LPUART1_Send(SET_ATID);
	Wait_For_OK();
	
	/* Set Destination address high */
	LPUART1_Send(SET_ATDH);
	Wait_For_OK();
	
	/* Set Destination Address low */
	LPUART1_Send(SET_ATDL);
	Wait_For_OK();
	
	/* Set Channel */
	LPUART1_Send(SET_ATCH);
	Wait_For_OK();
	
	/* Save the settings */
	LPUART1_Send(SAVE_SETTINGS);
	Wait_For_OK();
	
	/* End AT command mode */
	LPUART1_Send(EXIT_AT_COMMAND_MODE);
	Wait_For_OK();
	
	printf("#####  XBee 	       Initialized  #####\r\n");
}

/**
	\fn				void Read_Xbee_Init(void)
	\brief		Reads what the XBEE was initialized to
*/

void Read_Xbee_ProS1_Init(void){
	
	/* Enter AT command mode */
	Delay(1000);
	LPUART1_Send(ENTER_AT_COMMAND_MODE);
	
	/* Wait for XBee Acknowledge */
	Wait_For_OK();
	
	/* Read Personal Area Network */
	LPUART1_Send(READ_ATID);
	Wait_For_Data();
	strncpy(AT.ID,XBee_Message,6);
	
	/* Read MY Address */
	LPUART1_Send(READ_ATMY);
	Wait_For_Data();
	strncpy(AT.MY,XBee_Message,6);
	
	/* Read Destination address */
	LPUART1_Send(READ_ATDH);
	Wait_For_Data();
	strncpy(AT.DH,XBee_Message,9);
	
	LPUART1_Send(READ_ATDL);
	Wait_For_Data();
	strncpy(AT.DL,XBee_Message,9);
	
	/* Read Channel */
	LPUART1_Send(READ_ATCH);
	Wait_For_Data();
	strncpy(AT.CH,XBee_Message,4);
	
	/* End AT command mode */
	LPUART1_Send(EXIT_AT_COMMAND_MODE);
	Wait_For_OK();
	
	/* Print to serial monitor */
	printf("PAN: %s\r\n",AT.ID);
	printf("MY: %s\r\n",AT.MY);
	printf("DH: %s\r\n",AT.DH);
	printf("DL: %s\r\n",AT.DL);
	printf("CH: %s\r\n",AT.CH);
	
}

/**
	\fn				void Wait_For_OK(void)
	\brief		Waits until the XBEE has sent the OK message
						This is done when it has changed its settings
*/

void Wait_For_OK(void){
	
	/* Wait for XBee Acknowledge */
	while(Device_Ack_Flag == 0){
		//Nop
	}
	
	/* Reset Flags */
	Device_Ack_Flag = FALSE;
	XBee_Ready_To_Read = FALSE;
	
}

/**
	\fn				void Wait_For_Data(void)
	\brief		This waits for the data the XBEE writes to the bus
*/

void Wait_For_Data(void){
	
	/* Wait for data to be copied */
	while(XBee_Ready_To_Read == 0){
		//Nop
	}
	
	/* Reset Flags */
	Device_Ack_Flag = FALSE;
	XBee_Ready_To_Read = FALSE;
}

