/*  Copyright (s) 2019 ���ڰ������Ƽ����޹�˾
 *  All rights reserved
 * 
 * �ļ����ƣ�driver_usart.c
 * ժҪ��
 *  
 * �޸���ʷ     �汾��       Author       �޸�����
 *--------------------------------------------------
 * 2020.6.6      v01        ���ʿƼ�      �����ļ�
 *--------------------------------------------------
*/


/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include <stdio.h>

/* Library includes. */
#include "stm32f10x_lib.h"

/* Demo application includes. */
//#include "serial.h"
/*-----------------------------------------------------------*/



/*-----------------------------------------------------------*/

/* UART interrupt handler. */
void vUARTInterruptHandler( void );

/*-----------------------------------------------------------*/

/*
 * See the serial2.h header file.
 */
void UART_Init(unsigned long ulWantedBaud)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	
	/* Enable USART1 clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE );	

	/* Configure USART1 Rx (PA10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init( GPIOA, &GPIO_InitStructure );
	
	/* Configure USART1 Tx (PA9) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init( GPIOA, &GPIO_InitStructure );

	USART_InitStructure.USART_BaudRate = ulWantedBaud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_Clock = USART_Clock_Disable;
	USART_InitStructure.USART_CPOL = USART_CPOL_Low;
	USART_InitStructure.USART_CPHA = USART_CPHA_2Edge;
	USART_InitStructure.USART_LastBit = USART_LastBit_Disable;
	
	USART_Init( USART1, &USART_InitStructure );
	
	USART_ITConfig( USART1, USART_IT_RXNE, ENABLE );
	
	USART_Cmd( USART1, ENABLE );		
}
/*-----------------------------------------------------------*/



/*-----------------------------------------------------------*/

void vUARTInterruptHandler( void )
{
}


/*****************************************************
*function:	д�ַ��ļ�����
*param1:	������ַ�
*param2:	�ļ�ָ��
*return:	����ַ���ASCII��
******************************************************/
int fputc(int ch, FILE *f)
{
	while (!USART_GetFlagStatus(USART1, USART_FLAG_TXE));
	USART_SendData( USART1, ch );
	return ch;
}

/*****************************************************
*function:	���ַ��ļ�����
*param1:	�ļ�ָ��
*return:	��ȡ�ַ���ASCII��
******************************************************/
int fgetc(FILE *f)
{
    uint8_t ch = 0;
	while (!USART_GetFlagStatus(USART1, USART_FLAG_TC));
	ch = USART_ReceiveData( USART1 );
    return (int)ch;
}


/*
 * ������´��룬����Ҫ�ڹ��������й�ѡUse MicroLIB
*/

#pragma import(__use_no_semihosting)
 
struct __FILE
{
	int a;
};
 
FILE __stdout;
FILE __stdin;
void _sys_exit(int x)
{
	
}
