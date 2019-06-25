/***********************************************************************//**
 * @file		delay.c
 * @brief		Contains a function for SW time delay
 * @version		1.0
 * @date		18.02. 2012
 * @author		ACMC SW application team
 **********************************************************************/

#include "delay.h"

/*********************************************************************//**
 * @brief		delay function
 * @param[in]	iAsmValue
 * @return 		None
 **********************************************************************/
void delay_ms(unsigned int iAsmValue)
{
	int asmcounter;

	iAsmValue*=25000;

	asm volatile("mov	%0,%1" : "=r" (asmcounter) : "r" (iAsmValue));
	asm volatile("loop:");
	asm volatile("subs %0, %0, #1"  : "=r" (asmcounter));
	asm volatile ("bne loop");
}
