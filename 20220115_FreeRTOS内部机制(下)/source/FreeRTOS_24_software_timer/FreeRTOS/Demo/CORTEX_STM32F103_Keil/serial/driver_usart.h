/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_usart.h
 * 摘要：
 *  
 * 修改历史     版本号       Author       修改内容
 *--------------------------------------------------
 * 2020.6.6      v01        百问科技      创建文件
 *--------------------------------------------------
*/

#ifndef __DRIVER_USART_H
#define __DRIVER_USART_H

//#include "stm32f1xx_hal.h"

/*********************
 * 引脚宏定义
**********************/
#define USARTx                  USART1
#define USARTx_TX_PIN           GPIO_PIN_9
#define USARTx_RX_PIN           GPIO_PIN_10
#define USARTx_PORT             GPIOA
#define USARTx_GPIO_CLK_EN()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_CLK_EN()         __HAL_RCC_USART1_CLK_ENABLE()
#define USARTx_CLK_DIS()        __HAL_RCC_USART1_CLK_DISABLE()

/*********************
 * 函数宏定义
**********************/

/*********************
 * 全局变量声明
**********************/
extern UART_HandleTypeDef husart;

/*********************
 * 对外函数API
**********************/

/*
 *  函数名：void UsartInit(uint32_t baudrate)
 *  输入参数：baudrate-串口波特率
 *  输出参数：无
 *  返回值：无
 *  函数作用：初始化USART的波特率，收发选择，有效数据位等
*/
extern void UsartInit(uint32_t baudrate);
#endif
