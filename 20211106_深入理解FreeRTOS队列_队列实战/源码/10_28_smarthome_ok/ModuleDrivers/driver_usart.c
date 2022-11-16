#include "driver_usart.h"
#include "usart.h"
#include "main.h"
#include "ring_buffer.h"
#include <stdio.h>

static volatile uint8_t txcplt_flag = 0;
static volatile uint8_t rxcplt_flag = 0;

static volatile uint8_t rx_data = 0;

void EnableDebugIRQ(void)
{
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_TC | UART_IT_RXNE); 
}

void EnableUART3IRQ(void)
{
    HAL_NVIC_SetPriority(USART3_IRQn, 0, 1);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
    
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE); 
}



void DisableDebugIRQ(void)
{
    __HAL_UART_DISABLE_IT(&huart1, UART_IT_TC | UART_IT_RXNE);  
    
    HAL_NVIC_DisableIRQ(USART1_IRQn);
}

int fputc(int ch, FILE *f)
{
    txcplt_flag = 0;
    HAL_UART_Transmit_IT(&huart1, (uint8_t*)&ch, 1);
    while(txcplt_flag==0);
	return 0;
}

int fgetc(FILE *f)
{
    char c = 0;
    while(ring_buffer_read((unsigned char *)&c, &test_buffer) != 0);
    return c;
}

void USART1_IRQHandler(void)
{
    unsigned char c = 0;
    if((USART1->SR &(1<<5)) != 0)
    {
        c = USART1->DR;
        ring_buffer_write(c, &test_buffer);
    }
    HAL_UART_IRQHandler(&huart1);
}

typedef void (*NetInputProcessCallback)(char c);

static NetInputProcessCallback g_fNetInputProcessCallback;


/**********************************************************************
 * 函数名称： SetNetInputProcessCallback
 * 功能描述： 设置网络数据的处理回调函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0-成功, 其他值-失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/16	     V1.0	  韦东山	      创建
 ***********************************************************************/ 
void SetNetInputProcessCallback(NetInputProcessCallback func)
{
	g_fNetInputProcessCallback = func;
}


/**********************************************************************
 * 函数名称： USART3_IRQHandler
 * 功能描述： UART3中断函数,把接收到的数据放入环形缓冲区
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/15	     V1.0	  韦东山	      创建
 ***********************************************************************/
void USART3_IRQHandler(void)
{
	extern ring_buffer *GetUART3RingBuffer(void);
	
	static ring_buffer *uart3_ringbuffer = NULL;
    unsigned char c = 0;
	
	if (!uart3_ringbuffer)
		uart3_ringbuffer = GetUART3RingBuffer();
	
    if((USART3->SR &(1<<5)) != 0)
    {
        c = USART3->DR;
        ring_buffer_write(c, uart3_ringbuffer);
		if (g_fNetInputProcessCallback)
		{
			g_fNetInputProcessCallback(c);
		}
    }
    HAL_UART_IRQHandler(&huart3);
}

/**********************************************************************
 * 函数名称： USART3_SendBytes
 * 功能描述： 通过UART3发出多个数据
 * 输入参数： buf-数据缓冲区
 * 输入参数： len-数据长度
 * 输出参数： 无
 * 返 回 值： 0-成功
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/15	     V1.0	  韦东山	      创建
 ***********************************************************************/
void USART3_SendBytes(char *buf, int len)
{
	int i;
	for (i = 0; i < len; i++)
	{
		while ((USART3->SR & (1<<7)) == 0);
		USART3->DR = buf[i];		
	}
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        rxcplt_flag = 1;
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        txcplt_flag = 1;
    }
}

