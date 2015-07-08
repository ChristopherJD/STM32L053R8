/*-------------------------------------------------------------------------------------------------------------------
 * Name:    XBeePro24.c
 * Purpose: Initialize LPUSART, XBee, and send all data acquired
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
#define SAVE_SETTINGS									"ATWR\r"			//Saves the Settings you have configured
#define READ_ATMY											"ATMY\r"			//Read ATMY register
#define READ_ATID											"ATID\r"			//Read ATID register
#define READ_ATDH											"ATDH\r"			//Read ATDH register
#define READ_ATDL											"ATDL\r"			//Read ATDL register
/*---------------------------------More defines-----------------------------------------------------------------------*/
#define TRUE	1;
#define FALSE 0;
/*---------------------------------Globals----------------------------------------------------------------------------*/
char 										RX_Data[33] = "";							//Rx
uint8_t 								ChIndex = 0;									//Character Index
char 										XBee_Message[33] = "";				//Message Recieved by the XBee
static const char				OK[] = "OK";									//When data has been written XBee will send an OK
uint8_t									Device_Ack_Flag = FALSE;			//XBee OK acknowledge
uint8_t									XBee_Ready_To_Read = FALSE;		//XBee data ready
/*--------------------------------Struct Initialize-------------------------------------------------------------------*/
AT_Data AT;
/*--------------------------------Functions---------------------------------------------------------------------------*/
void Wait_For_OK(void);
void Wait_For_Data(void);

/**
	\fn			void RNG_LPUART1_IRQHandler(void)
	\breif	Global interrupt handler for LPUART, Currently only handles RX
*/
void RNG_LPUART1_IRQHandler(void){
	if((LPUART1->ISR & USART_ISR_RXNE) == USART_ISR_RXNE){
		
		/* Read RX Data */
		RX_Data[ChIndex] = LPUART1->RDR;
		
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

void LPUART_Init(void){
	
	RCC->IOPENR   |=   RCC_IOPENR_GPIOCEN;			/* Enable GPIOB clock */
	RCC->APB1ENR  |=   RCC_APB1ENR_LPUART1EN;   /* Enable LP USART#1 clock */
	
	//interrupt init
	NVIC_EnableIRQ(RNG_LPUART1_IRQn);
	NVIC_SetPriority(RNG_LPUART1_IRQn,1);
	
	//Configure PC10 to LPUSART1_TX, PC11 to LPUSART1_RX,PC10 and PC11 TX,RX AF0 by default
  GPIOC->MODER  &= ~(( 3ul << 2* 10) | ( 3ul << 2* 11) );		/* Set to 0 */
  GPIOC->MODER  |=  (( 2ul << 2* 10) | ( 2ul << 2* 11) );		/* Set to alternate function mode */
	
	LPUART1->BRR  = 0xD0555;  																/* 9600 baud @ 32MHz */
  LPUART1->CR3    = 0x0000;																	/* no flow control */
  LPUART1->CR1    = ((USART_CR1_RE) |												/* enable RX  */
                    (USART_CR1_TE) |												/* enable TX  */
                    (   0ul << 12) |												/* 8 data bits */
                    (   0ul << 28) |												/* 8 data bits */
                    (USART_CR1_UE) |      									/* enable USART */
										(USART_CR1_RXNEIE));										/* Enable Interrupt */
										
}

char LPUART1_PutChar(char ch) {

	//Wait for buffer to be empty
  while ((LPUART1->ISR & USART_ISR_TXE) == 0){
			//Nop
	}
	
	//Send character
  LPUART1->TDR = (ch);

  return (ch);
}

void LPUART1_Send(char c[]){
	
	int String_Length = strlen(c);
	int Counter = 0;
	
	while(Counter < String_Length){
		LPUART1_PutChar(c[Counter]);
		Counter++;
	}
}

void XBee_Init(void){
	
	/* Enter AT command mode */
	Delay(1000);
	LPUART1_Send(ENTER_AT_COMMAND_MODE);
	Wait_For_OK();
	
	/* MY address = 2 */
	LPUART1_Send(SET_ATMY);
	Wait_For_OK();
	
	/* PAL = 3001 */
	LPUART1_Send(SET_ATID);
	Wait_For_OK();
	
	/* Set Destination address high */
	LPUART1_Send(SET_ATDH);
	Wait_For_OK();
	
	/* Set Destination Address low */
	LPUART1_Send(SET_ATDL);
	Wait_For_OK();
	
	/* End AT command mode */
	LPUART1_Send(EXIT_AT_COMMAND_MODE);
	Wait_For_OK();
	
	printf("#####  XBee Initialized             #####\r\n");
}

void Read_Xbee_Init(void){
	
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
	
	/* End AT command mode */
	LPUART1_Send(EXIT_AT_COMMAND_MODE);
	Wait_For_OK();
	
	/* Print to serial monitor */
	printf("PAN: %s\r\n",AT.ID);
	printf("MY: %s\r\n",AT.MY);
	printf("DH: %s\r\n",AT.DH);
	printf("DL: %s\r\n",AT.DL);
	
}

void Wait_For_OK(void){
	
	/* Wait for XBee Acknowledge */
	while(Device_Ack_Flag == 0){
		//Nop
	}
	
	/* Reset Flags */
	Device_Ack_Flag = FALSE;
	XBee_Ready_To_Read = FALSE;
	
}

void Wait_For_Data(void){
	
	/* Wait for data to be copied */
	while(XBee_Ready_To_Read == 0){
		//Nop
	}
	
	/* Reset Flags */
	Device_Ack_Flag = FALSE;
	XBee_Ready_To_Read = FALSE;
}

