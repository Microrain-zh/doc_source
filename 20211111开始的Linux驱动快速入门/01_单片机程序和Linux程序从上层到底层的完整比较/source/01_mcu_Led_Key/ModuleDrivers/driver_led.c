/** @file		driver_led.c
  * @author 	百问网团队
  * @version	v1.0
  * @date		2021-8-10
  * *******************************************
  * @brief		LED驱动源文件
  * 			+
  *
  * @include	包含的头文件
  * 			+ main.h
  * 			+ driver_led.h
  * -------------------------------------------
  * *******************************************
  * -------------------------------------------
  * @attention
  *
  * Copyright (c) 深圳百问网科技有限公司
  * All rights reserved.
  *
  */

#include "driver_led.h"
#include "main.h"

/*
 *  函数名：BlueOn
 *  功能描述：PF8输出低电平，点亮蓝色LED
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
void BlueOn(void)
{
    HAL_GPIO_WritePin(BLUE_GPIO_Port, BLUE_Pin, 0);
}

/*
 *  函数名：BlueOff
 *  功能描述：PF8输出高电平，熄灭蓝色LED
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
void BlueOff(void)
{
    HAL_GPIO_WritePin(BLUE_GPIO_Port, BLUE_Pin, 1);
}

/*
 *  函数名：BlueShine
 *  功能描述：PF8输出高低电平变换，闪烁蓝色LED
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
void BlueShine(void)
{
    BlueOn();
    HAL_Delay(500);
    BlueOff();
    HAL_Delay(500);
}

/*
 *  函数名：BlueShine_2
 *  功能描述：使用HAL_GPIO_TogglePin函数改变PF8的电平，闪烁蓝色LED
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
void BlueShine_2(void)
{
    HAL_GPIO_TogglePin(BLUE_GPIO_Port, BLUE_Pin);
}

