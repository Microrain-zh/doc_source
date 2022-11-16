/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_usart.c
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2021.8.21      v01         百问科技      创建文件
 *--------------------------------------------------
*/

#include "driver_usart.h"
#include "usart.h"
#include "main.h"
#include "ring_buffer.h"
#include <stdio.h>

static volatile uint8_t txcplt_flag = 0;    // 发送完成标志，1完成0未完成
static volatile uint8_t rxcplt_flag = 0;    // 接收完成标志，1完成0未完成

static volatile uint8_t rx_data = 0;

/*
 *  函数名：void EnableDebugIRQ(void)
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
 *  函数作用：使能USART1的中断
*/
void EnableDebugIRQ(void)
{
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);    // 设置USART1中断的优先级
    HAL_NVIC_EnableIRQ(USART1_IRQn);            // 使能USART1的中断
    
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_TC | UART_IT_RXNE);   // 使能USRAT1的发送和接收中断
}

/*
 *  函数名：void DisableDebugIRQ(void)
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
 *  函数作用：失能USART1的中断
*/
void DisableDebugIRQ(void)
{
    __HAL_UART_DISABLE_IT(&huart1, UART_IT_TC | UART_IT_RXNE);      // 失能USRAT1的发送和接收中断
    
    HAL_NVIC_DisableIRQ(USART1_IRQn);   // 失能USART1的中断
}

/*
 *  函数名：int fputc(int ch, FILE *f)
 *  输入参数：ch --> 要输出的数据
 *  输出参数：无
 *  返回值：无
 *  函数作用：printf/putchar 标准输出函数的底层输出函数
*/
int fputc(int ch, FILE *f)
{
    txcplt_flag = 0;
    HAL_UART_Transmit_IT(&huart1, (uint8_t*)&ch, 1);
    while(txcplt_flag==0);
}

/*
 *  函数名：int fgetc(FILE *f)
 *  输入参数：
 *  输出参数：无
 *  返回值：接收到的数据
 *  函数作用：scanf/getchar 标准输出函数的底层输出函数
*/
int fgetc(FILE *f)
{
    char c = 0;
    while(ring_buffer_read((unsigned char *)&c, &test_buffer) != 0);
    return c;
}

/*
 *  函数名：void USART1_IRQHandler(void)
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
 *  函数作用：USART1的中断服务函数
*/
void USART1_IRQHandler(void)
{
    unsigned char c = 0;
    if((USART1->SR &(1<<5)) != 0)   // 判断USART1的状态寄存器的第五位即RXNE位是否被置位
    {
        c = USART1->DR; // RXNE=1，表明DR寄存器有值，就将它读出来保存到临时变量中；
        ring_buffer_write(c, &test_buffer); // 将数据保存到环形缓冲区中
    }
    HAL_UART_IRQHandler(&huart1);   // HAL库中的UART统一中断服务函数，通过形参判断是要处理谁的中断
}

/*
 *  函数名：void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
 *  输入参数：huart --> UART的设备句柄，用以指明UART设备是哪一个UART
 *  输出参数：无
 *  返回值：无
 *  函数作用：HAL库中的UART接收完成回调函数
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)   // 判断进来的是否是USART1这个UART设备
    {
        rxcplt_flag = 1;    // 进入此回调函数表明接收指定长度的数据已经完成，将标志置一
    }
}

/*
 *  函数名：void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
 *  输入参数：huart --> UART的设备句柄，用以指明UART设备是哪一个UART
 *  输出参数：无
 *  返回值：无
*  函数作用：HAL库中的UART发送完成回调函数
*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)   // 判断进来的是否是USART1这个UART设备
    {
        txcplt_flag = 1;    // 进入此回调函数表明发送指定长度的数据已经完成，将标志置一
    }
}