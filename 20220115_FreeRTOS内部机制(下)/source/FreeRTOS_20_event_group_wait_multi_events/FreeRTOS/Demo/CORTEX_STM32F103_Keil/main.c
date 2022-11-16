/*
 * FreeRTOS V202104.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the standard demo application tasks.
 * In addition to the standard demo tasks, the following tasks and tests are
 * defined and/or created within this file:
 *
 * "Fast Interrupt Test" - A high frequency periodic interrupt is generated
 * using a free running timer to demonstrate the use of the
 * configKERNEL_INTERRUPT_PRIORITY configuration constant.  The interrupt
 * service routine measures the number of processor clocks that occur between
 * each interrupt - and in so doing measures the jitter in the interrupt timing.
 * The maximum measured jitter time is latched in the ulMaxJitter variable, and
 * displayed on the LCD by the 'Check' task as described below.  The
 * fast interrupt is configured and handled in the timertest.c source file.
 *
 * "LCD" task - the LCD task is a 'gatekeeper' task.  It is the only task that
 * is permitted to access the display directly.  Other tasks wishing to write a
 * message to the LCD send the message on a queue to the LCD task instead of
 * accessing the LCD themselves.  The LCD task just blocks on the queue waiting
 * for messages - waking and displaying the messages as they arrive.
 *
 * "Check" task -  This only executes every five seconds but has the highest
 * priority so is guaranteed to get processor time.  Its main function is to
 * check that all the standard demo tasks are still operational.  Should any
 * unexpected behaviour within a demo task be discovered the 'check' task will
 * write an error to the LCD (via the LCD task).  If all the demo tasks are
 * executing with their expected behaviour then the check task writes PASS
 * along with the max jitter time to the LCD (again via the LCD task), as
 * described above.
 *
 */

/* Standard includes. */
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

/* Library includes. */
#include "stm32f10x_it.h"

extern 	void UART_Init(unsigned long ulWantedBaud);

/* Demo app includes. */
static void prvSetupHardware( void );


/*-----------------------------------------------------------*/

static void vWashingTask( void *pvParameters );
static void vFiringTask( void *pvParameters );
static void vCookingTask( void *pvParameters );

/*-----------------------------------------------------------*/

/* 事件组句柄 */
EventGroupHandle_t xEventGroup;

/* bit0: 洗菜
 * bit1: 生火
 * bit2: 炒菜
 */
#define WASHING  (1<<0)
#define FIRING   (1<<1)
#define COOKING  (1<<2)

int main( void )
{
	prvSetupHardware();
	
    /* 创建递归锁 */
    xEventGroup = xEventGroupCreate( );

	if( xEventGroup != NULL )
	{
		/* 创建3个任务: 洗菜/生火/炒菜
		 */
		xTaskCreate( vWashingTask, "Task1", 1000, NULL, 1, NULL );
		xTaskCreate( vFiringTask,  "Task2", 1000, NULL, 2, NULL );
		xTaskCreate( vCookingTask, "Task3", 1000, NULL, 3, NULL );

		/* 启动调度器 */
		vTaskStartScheduler();
	}
	else
	{
		/* 无法创建事件组 */
	}

	/* 如果程序运行到了这里就表示出错了, 一般是内存不足 */
	return 0;
}

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/
static void vWashingTask( void *pvParameters )
{
	int i = 0;
	
	/* 无限循环 */
	for( ;; )
	{
		printf("I am washing %d time....\r\n", i++);
		
		/* 发出事件: 我洗完菜了 */
		xEventGroupSetBits(xEventGroup, WASHING);	
		
		/* 等待大厨炒完菜, 再继续洗菜 */
		xEventGroupWaitBits(xEventGroup, COOKING, pdTRUE, pdTRUE, portMAX_DELAY);
	}
}

static void vFiringTask( void *pvParameters )
{
	int i = 0;
	
	/* 无限循环 */
	for( ;; )
	{
		/* 等待洗完菜, 才生火 */
		xEventGroupWaitBits(xEventGroup, WASHING, pdFALSE, pdTRUE, portMAX_DELAY);
		
		printf("I am firing %d time....\r\n", i++);

		/* 发出事件: 我生好火了 */
		xEventGroupSetBits(xEventGroup, FIRING);			
	}
}

static void vCookingTask( void *pvParameters )
{
	int i = 0;
	
	/* 无限循环 */
	for( ;; )
	{
		/* 等待2件事: 洗菜, 生火 */
		xEventGroupWaitBits(xEventGroup, WASHING|FIRING, pdTRUE, pdTRUE, portMAX_DELAY);
		
		printf("I am cooking %d time....\r\n", i++);
		
		/* 发出事件: 我炒好菜了 */
		xEventGroupSetBits(xEventGroup, COOKING);			
	}
}

/*-----------------------------------------------------------*/


/*-----------------------------------------------------------*/



static void prvSetupHardware( void )
{
	/* Start with the clocks in their expected state. */
	RCC_DeInit();

	/* Enable HSE (high speed external clock). */
	RCC_HSEConfig( RCC_HSE_ON );

	/* Wait till HSE is ready. */
	while( RCC_GetFlagStatus( RCC_FLAG_HSERDY ) == RESET )
	{
	}

	/* 2 wait states required on the flash. */
	*( ( unsigned long * ) 0x40022000 ) = 0x02;

	/* HCLK = SYSCLK */
	RCC_HCLKConfig( RCC_SYSCLK_Div1 );

	/* PCLK2 = HCLK */
	RCC_PCLK2Config( RCC_HCLK_Div1 );

	/* PCLK1 = HCLK/2 */
	RCC_PCLK1Config( RCC_HCLK_Div2 );

	/* PLLCLK = 8MHz * 9 = 72 MHz. */
	RCC_PLLConfig( RCC_PLLSource_HSE_Div1, RCC_PLLMul_9 );

	/* Enable PLL. */
	RCC_PLLCmd( ENABLE );

	/* Wait till PLL is ready. */
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{
	}

	/* Select PLL as system clock source. */
	RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );

	/* Wait till PLL is used as system clock source. */
	while( RCC_GetSYSCLKSource() != 0x08 )
	{
	}

	/* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE and AFIO clocks */
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC
							| RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE );

	/* SPI2 Periph clock enable */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE );


	/* Set the Vector Table base address at 0x08000000 */
	NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x0 );

	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	/* Configure HCLK clock as SysTick clock source. */
	SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );

	//vParTestInitialise();
	
	UART_Init(115200);
}
/*-----------------------------------------------------------*/

#ifdef  DEBUG
/* Keep the linker happy. */
void assert_failed( unsigned char* pcFile, unsigned long ulLine )
{
	for( ;; )
	{
	}
}
#endif
