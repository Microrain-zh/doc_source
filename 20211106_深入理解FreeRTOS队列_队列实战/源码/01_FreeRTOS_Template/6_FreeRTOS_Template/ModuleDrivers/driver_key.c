/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_key.c
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2021.8.21      v01         百问科技      创建文件
 *--------------------------------------------------
*/

#include "driver_key.h"

static volatile uint8_t key1_val = KEY_RELEASED;     // 按键KEY1的键值，按下为0松开为1
static volatile uint8_t key2_val = KEY_RELEASED;     // 按键KEY2的键值，按下为0松开为1

/*
 *  函数名：void KEY_GPIO_ReInit(void)
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
 *  函数作用：按键的GPIO重新初始化为双边沿触发外部中断
*/
void KEY_GPIO_ReInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	KEY1_GPIO_CLK_EN();
	KEY2_GPIO_CLK_EN();
    
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;     // 双边沿触发中断
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	
	GPIO_InitStruct.Pin = KEY1_PIN;
	HAL_GPIO_Init(KEY1_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = KEY2_PIN;
	HAL_GPIO_Init(KEY1_PORT, &GPIO_InitStruct);
	
	
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 2);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/*
 *  函数名：void EXTI15_10_IRQHandler(void)
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
 *  函数作用：外部中断线10~15的中断服务函数
*/
void EXTI15_10_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(KEY1_PIN); // 使用HAL库的外部中断统一服务函数，参数是外部中断线
	HAL_GPIO_EXTI_IRQHandler(KEY2_PIN);
}

/*
 *  函数名：void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
 *  输入参数：GPIO_Pin --> 产生外部中断的引脚
 *  输出参数：无
 *  返回值：无
 *  函数作用：外部中断回调函数
*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(KEY1_PIN == GPIO_Pin)    // 判断进来的外部中断线连接的引脚是不是按键的引脚
	{
		key1_val = K1;  // 读取按键连接的引脚的状态
	}
	else if(KEY2_PIN == GPIO_Pin)
	{
		key2_val = K2;
	}
}

/*
 *  函数名：uint8_t KEY1_Value(void)
 *  输入参数：无
 *  输出参数：无
 *  返回值：返回按键KEY1的键值
 *  函数作用：返回按键KEY1的键值
*/
uint8_t KEY1_Value(void)
{
	return key1_val;
}

/*
 *  函数名：uint8_t KEY2_Value(void)
 *  输入参数：无
 *  输出参数：无
 *  返回值：返回按键KEY2的键值
 *  函数作用：返回按键KEY2的键值
*/
uint8_t KEY2_Value(void)
{
	return key2_val;
}