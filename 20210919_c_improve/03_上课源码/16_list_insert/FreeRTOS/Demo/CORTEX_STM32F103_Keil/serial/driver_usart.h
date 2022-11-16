/*  Copyright (s) 2019 ���ڰ������Ƽ����޹�˾
 *  All rights reserved
 * 
 * �ļ����ƣ�driver_usart.h
 * ժҪ��
 *  
 * �޸���ʷ     �汾��       Author       �޸�����
 *--------------------------------------------------
 * 2020.6.6      v01        ���ʿƼ�      �����ļ�
 *--------------------------------------------------
*/

#ifndef __DRIVER_USART_H
#define __DRIVER_USART_H

//#include "stm32f1xx_hal.h"

/*********************
 * ���ź궨��
**********************/
#define USARTx                  USART1
#define USARTx_TX_PIN           GPIO_PIN_9
#define USARTx_RX_PIN           GPIO_PIN_10
#define USARTx_PORT             GPIOA
#define USARTx_GPIO_CLK_EN()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_CLK_EN()         __HAL_RCC_USART1_CLK_ENABLE()
#define USARTx_CLK_DIS()        __HAL_RCC_USART1_CLK_DISABLE()

/*********************
 * �����궨��
**********************/

/*********************
 * ȫ�ֱ�������
**********************/
extern UART_HandleTypeDef husart;

/*********************
 * ���⺯��API
**********************/

/*
 *  ��������void UsartInit(uint32_t baudrate)
 *  ���������baudrate-���ڲ�����
 *  �����������
 *  ����ֵ����
 *  �������ã���ʼ��USART�Ĳ����ʣ��շ�ѡ����Ч����λ��
*/
extern void UsartInit(uint32_t baudrate);
#endif
