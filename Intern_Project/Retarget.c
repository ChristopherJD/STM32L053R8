/*------------------------------------------------------------------------------------------------------
 * Name:    Retarget.c
 * Purpose: Retargets printf C function to be used with USART2
 * Date: 		7/14/15
 * Author:	Christopher Jordan - Denny
 *------------------------------------------------------------------------------------------------------
 * Note(s):	This is really just magic
 *----------------------------------------------------------------------------------------------------*/

/*--------------------------------------Include Statements--------------------------------------------*/
#include <stdio.h>
#include <rt_misc.h>
#include "Serial.h"
/*--------------------------------------Additional Compiler Info--------------------------------------*/
#pragma import(__use_no_semihosting_swi)
/*--------------------------------------Structure Definitions-----------------------------------------*/
struct __FILE { int handle;};
FILE __stdout;
FILE __stdin;
/*--------------------------------------Functions-----------------------------------------------------*/

int fputc(int c, FILE *f){
  return (SER_PutChar(c));
}


int fgetc(FILE *f){
  return (SER_GetChar());
}

void _sys_exit(int return_code){
label:  goto label;  /* endless loop */
}
