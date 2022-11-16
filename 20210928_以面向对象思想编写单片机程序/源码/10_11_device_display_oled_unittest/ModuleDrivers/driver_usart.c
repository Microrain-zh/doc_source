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
