/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_key.h
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2020.6.6      v01         百问科技      创建文件
 *--------------------------------------------------
*/
#ifndef __DRIVER_KEY_H
#define __DRIVER_KEY_H

#include "stm32f1xx_hal.h"

/*********************
 * 函数宏定义
**********************/
#define K1      HAL_GPIO_ReadPin(K1_GPIO_Port, K1_Pin)

/*
 *  函数名：K1_Value
 *  功能描述：获取按键KEY1的状态，按下返回0，否则返回1
 *  输入参数：无
 *  输出参数：无
 *  返回值：按键KEY1的状态
*/
extern uint8_t K1_Value(void);

#endif /* __DRIVER_KEY_H */