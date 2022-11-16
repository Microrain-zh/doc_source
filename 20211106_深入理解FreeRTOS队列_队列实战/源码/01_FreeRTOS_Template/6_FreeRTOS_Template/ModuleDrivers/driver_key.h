/*  Copyright (s) 2019 ���ڰ������Ƽ����޹�˾
 *  All rights reserved
 * 
 * �ļ����ƣ�driver_key.h
 * ժҪ��
 *  
 * �޸���ʷ     �汾��        Author       �޸�����
 *--------------------------------------------------
 * 2021.8.21      v01         ���ʿƼ�      �����ļ�
 *--------------------------------------------------
*/

#ifndef __DRIVER_KEY_H
#define __DRIVER_KEY_H

#include "stm32f1xx_hal.h"

/*
    ����״̬�궨��
*/
#define KEY_PRESSED			(0)
#define KEY_RELEASED		(1)

/*********************
 * ���ź궨��
**********************/
#define KEY1_PIN				GPIO_PIN_14
#define KEY2_PIN				GPIO_PIN_15

#define KEY1_PORT				GPIOE
#define KEY2_PORT				GPIOE

/*********************
 * �����궨��
**********************/
#define KEY1_GPIO_CLK_EN()	__HAL_RCC_GPIOE_CLK_ENABLE()
#define KEY2_GPIO_CLK_EN()	__HAL_RCC_GPIOE_CLK_ENABLE()

#define K1							HAL_GPIO_ReadPin(KEY1_PORT, KEY1_PIN)
#define K2							HAL_GPIO_ReadPin(KEY2_PORT, KEY2_PIN)

/*
 *  ��������void KEY_GPIO_ReInit(void)
 *  �����������
 *  �����������
 *  ����ֵ����
 *  �������ã�������GPIO���³�ʼ��Ϊ˫���ش����ⲿ�ж�
*/
extern void KEY_GPIO_ReInit(void);

/*
 *  ��������uint8_t KEY1_Value(void)
 *  �����������
 *  �����������
 *  ����ֵ�����ذ���KEY1�ļ�ֵ
 *  �������ã����ذ���KEY1�ļ�ֵ
*/
extern uint8_t KEY1_Value(void);

/*
 *  ��������uint8_t KEY2_Value(void)
 *  �����������
 *  �����������
 *  ����ֵ�����ذ���KEY2�ļ�ֵ
 *  �������ã����ذ���KEY2�ļ�ֵ
*/
extern uint8_t KEY2_Value(void);

#endif /* __DRIVER_KEY_H */