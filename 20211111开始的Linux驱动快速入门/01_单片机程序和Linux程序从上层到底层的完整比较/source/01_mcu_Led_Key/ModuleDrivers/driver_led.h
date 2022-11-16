/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_led.h
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2020.6.6      v01         百问科技      创建文件
 *--------------------------------------------------
*/

#ifndef __DRIVER_LED_H
#define __DRIVER_LED_H

/*********************
 * 函数宏定义
**********************/
#define BLUE_ON()       HAL_GPIO_WritePin(BLUE_GPIO_Port, BLUE_Pin, 0)
#define BLUE_OFF()      HAL_GPIO_WritePin(BLUE_GPIO_Port, BLUE_Pin, 1)  

/*
 *  函数名：BlueOn
 *  功能描述：PF8输出低电平，点亮蓝色LED
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
extern void BlueOn(void);

/*
 *  函数名：BlueOff
 *  功能描述：PF8输出高电平，熄灭蓝色LED
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
extern void BlueOff(void);

/*
 *  函数名：BlueShine
 *  功能描述：PF8输出高低电平变换，闪烁蓝色LED
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
extern void BlueShine(void);

/*
 *  函数名：BlueShine_2
 *  功能描述：使用HAL_GPIO_TogglePin函数改变PF8的电平，闪烁蓝色LED
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
extern void BlueShine_2(void);

#endif /* __DRIVER_LED_H */