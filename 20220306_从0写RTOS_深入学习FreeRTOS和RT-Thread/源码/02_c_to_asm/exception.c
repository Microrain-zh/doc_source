
#include "uart.h"
#include "exception.h"

void HardFault_Handler(void)
{
	puts("HardFault_Handler\n\r");
	while (1);
}

void UsageFault_Handler(unsigned int *stack)
{
	SCB_Type * SCB = (SCB_Type *)SCB_BASE_ADDR;
	
	puts("UsageFault_Handler\n\r");
	put_s_hex("R0  = ", stack[0]);
	put_s_hex("R1  = ", stack[1]);
	put_s_hex("R2  = ", stack[2]);
	put_s_hex("R3  = ", stack[3]);
	put_s_hex("R12 = ", stack[4]);
	put_s_hex("LR  = ", stack[5]);
	put_s_hex("PC  = ", stack[6]);
	put_s_hex("xPSR= ", stack[7]);
	
	/* 1. clear usage fault */
	SCB->CFSR = SCB->CFSR;
	
	/* 2. set return address in sp, piont to next instruction */
	stack[6] += 4;
}

void SVC_Handler(void)
{
	puts("SVC_Handler\n\r");
}

void UsageFaultInit(void)
{
	SCB_Type * SCB = (SCB_Type *)SCB_BASE_ADDR;
	SCB->SHCSR |= (SCB_SHCSR_USGFAULTENA_Msk);
}