/*  Copyright (s) 2019 ���ڰ������Ƽ����޹�˾
 *  All rights reserved
 * 
 * �ļ����ƣ�driver_key.c
 * ժҪ��
 *  
 * �޸���ʷ     �汾��        Author       �޸�����
 *--------------------------------------------------
 * 2021.8.21      v01         ���ʿƼ�      �����ļ�
 *--------------------------------------------------
*/

#include "driver_key.h"

static volatile uint8_t key1_val = KEY_RELEASED;     // ����KEY1�ļ�ֵ������Ϊ0�ɿ�Ϊ1
static volatile uint8_t key2_val = KEY_RELEASED;     // ����KEY2�ļ�ֵ������Ϊ0�ɿ�Ϊ1

/*
 *  ��������void KEY_GPIO_ReInit(void)
 *  �����������
 *  �����������
 *  ����ֵ����
 *  �������ã�������GPIO���³�ʼ��Ϊ˫���ش����ⲿ�ж�
*/
void KEY_GPIO_ReInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	KEY1_GPIO_CLK_EN();
	KEY2_GPIO_CLK_EN();
    
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;     // ˫���ش����ж�
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	
	GPIO_InitStruct.Pin = KEY1_PIN;
	HAL_GPIO_Init(KEY1_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = KEY2_PIN;
	HAL_GPIO_Init(KEY1_PORT, &GPIO_InitStruct);
	
	
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 2);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/*
 *  ��������void EXTI15_10_IRQHandler(void)
 *  �����������
 *  �����������
 *  ����ֵ����
 *  �������ã��ⲿ�ж���10~15���жϷ�����
*/
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(KEY1_PIN); // ʹ��HAL����ⲿ�ж�ͳһ���������������ⲿ�ж���
	HAL_GPIO_EXTI_IRQHandler(KEY2_PIN);
}

/*
 *  ��������void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
 *  ���������GPIO_Pin --> �����ⲿ�жϵ�����
 *  �����������
 *  ����ֵ����
 *  �������ã��ⲿ�жϻص�����
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(KEY1_PIN == GPIO_Pin)    // �жϽ������ⲿ�ж������ӵ������ǲ��ǰ���������
	{
		key1_val = K1;  // ��ȡ�������ӵ����ŵ�״̬
	}
	else if(KEY2_PIN == GPIO_Pin)
	{
		key2_val = K2;
	}
}

/*
 *  ��������uint8_t KEY1_Value(void)
 *  �����������
 *  �����������
 *  ����ֵ�����ذ���KEY1�ļ�ֵ
 *  �������ã����ذ���KEY1�ļ�ֵ
*/
uint8_t KEY1_Value(void)
{
	return key1_val;
}

/*
 *  ��������uint8_t KEY2_Value(void)
 *  �����������
 *  �����������
 *  ����ֵ�����ذ���KEY2�ļ�ֵ
 *  �������ã����ذ���KEY2�ļ�ֵ
*/
uint8_t KEY2_Value(void)
{
	return key2_val;
}