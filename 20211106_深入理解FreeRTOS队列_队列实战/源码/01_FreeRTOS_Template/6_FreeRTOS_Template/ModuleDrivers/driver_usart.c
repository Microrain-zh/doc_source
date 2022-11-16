/*  Copyright (s) 2019 ���ڰ������Ƽ����޹�˾
 *  All rights reserved
 * 
 * �ļ����ƣ�driver_usart.c
 * ժҪ��
 *  
 * �޸���ʷ     �汾��        Author       �޸�����
 *--------------------------------------------------
 * 2021.8.21      v01         ���ʿƼ�      �����ļ�
 *--------------------------------------------------
*/

#include "driver_usart.h"
#include "usart.h"
#include "main.h"
#include "ring_buffer.h"
#include <stdio.h>

static volatile uint8_t txcplt_flag = 0;    // ������ɱ�־��1���0δ���
static volatile uint8_t rxcplt_flag = 0;    // ������ɱ�־��1���0δ���

static volatile uint8_t rx_data = 0;

/*
 *  ��������void EnableDebugIRQ(void)
 *  �����������
 *  �����������
 *  ����ֵ����
 *  �������ã�ʹ��USART1���ж�
*/
void EnableDebugIRQ(void)
{
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);    // ����USART1�жϵ����ȼ�
    HAL_NVIC_EnableIRQ(USART1_IRQn);            // ʹ��USART1���ж�
    
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_TC | UART_IT_RXNE);   // ʹ��USRAT1�ķ��ͺͽ����ж�
}

/*
 *  ��������void DisableDebugIRQ(void)
 *  �����������
 *  �����������
 *  ����ֵ����
 *  �������ã�ʧ��USART1���ж�
*/
void DisableDebugIRQ(void)
{
    __HAL_UART_DISABLE_IT(&huart1, UART_IT_TC | UART_IT_RXNE);      // ʧ��USRAT1�ķ��ͺͽ����ж�
    
    HAL_NVIC_DisableIRQ(USART1_IRQn);   // ʧ��USART1���ж�
}

/*
 *  ��������int fputc(int ch, FILE *f)
 *  ���������ch --> Ҫ���������
 *  �����������
 *  ����ֵ����
 *  �������ã�printf/putchar ��׼��������ĵײ��������
*/
int fputc(int ch, FILE *f)
{
    txcplt_flag = 0;
    HAL_UART_Transmit_IT(&huart1, (uint8_t*)&ch, 1);
    while(txcplt_flag==0);
}

/*
 *  ��������int fgetc(FILE *f)
 *  ���������
 *  �����������
 *  ����ֵ�����յ�������
 *  �������ã�scanf/getchar ��׼��������ĵײ��������
*/
int fgetc(FILE *f)
{
    char c = 0;
    while(ring_buffer_read((unsigned char *)&c, &test_buffer) != 0);
    return c;
}

/*
 *  ��������void USART1_IRQHandler(void)
 *  �����������
 *  �����������
 *  ����ֵ����
 *  �������ã�USART1���жϷ�����
*/
void USART1_IRQHandler(void)
{
    unsigned char c = 0;
    if((USART1->SR &(1<<5)) != 0)   // �ж�USART1��״̬�Ĵ����ĵ���λ��RXNEλ�Ƿ���λ
    {
        c = USART1->DR; // RXNE=1������DR�Ĵ�����ֵ���ͽ������������浽��ʱ�����У�
        ring_buffer_write(c, &test_buffer); // �����ݱ��浽���λ�������
    }
    HAL_UART_IRQHandler(&huart1);   // HAL���е�UARTͳһ�жϷ�������ͨ���β��ж���Ҫ����˭���ж�
}

/*
 *  ��������void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
 *  ���������huart --> UART���豸���������ָ��UART�豸����һ��UART
 *  �����������
 *  ����ֵ����
 *  �������ã�HAL���е�UART������ɻص�����
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)   // �жϽ������Ƿ���USART1���UART�豸
    {
        rxcplt_flag = 1;    // ����˻ص�������������ָ�����ȵ������Ѿ���ɣ�����־��һ
    }
}

/*
 *  ��������void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
 *  ���������huart --> UART���豸���������ָ��UART�豸����һ��UART
 *  �����������
 *  ����ֵ����
*  �������ã�HAL���е�UART������ɻص�����
*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)   // �жϽ������Ƿ���USART1���UART�豸
    {
        txcplt_flag = 1;    // ����˻ص�������������ָ�����ȵ������Ѿ���ɣ�����־��һ
    }
}