/*------------------------------------------------------------------------------------------------------
 * Name:    Timing.c
 * Purpose: Sets up the system clock and also has a delay function
 * Date: 		7/14/15
 * Author:	Christopher Jordan - Denny
 *------------------------------------------------------------------------------------------------------
 * Note(s):
 *----------------------------------------------------------------------------------------------------*/

#ifndef Timing_H
#define Timing_H

extern void SystemCoreClockInit(void);
extern void Delay (unsigned int dlyTicks);
extern void Start_15s_Timer(void);

#endif


