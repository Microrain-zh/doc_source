#include "uart.h"
#include "exception.h"
#include "systick.h"
#include "led.h"


void SysTickInit(void)
{
	SysTick_Type *SysTick = (SysTick_Type *)SysTick_BASE;
	/* 1. set period : 1S  */
	/* 2. set Load */
	SysTick->VAL  = SYSTICK_FRE;
	SysTick->LOAD = SYSTICK_FRE;
	
	/* 3. select clk source, enable systick, enable exception 
	 *  BIT[2] : clk source, process clk
	 *  BIT[1] : enable exception
	 *  BIT[0] : enable systick
	 */
	SysTick->CTRL = (1<<2) | (1<<1) | (1<<0);
}

void SysTick_Handler(void)
{
	SCB_Type * SCB = (SCB_Type *)SCB_BASE_ADDR;
	
	static int led_on = 0;
	/* led */
	if (led_on)
	{
		/* let led off */
		LedCtrl(0);
	}
	else
	{
		/* let led on */
		LedCtrl(1);
	}
	
	led_on = !led_on;
	
	/* clear exception status */
	SCB->ICSR |= SCB_ICSR_PENDSTCLR_Msk;
}