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

#include <led_device.h>

/*
 *  函数名：int HAL_LEDDeviceControl(struct LEDDevice *ptLEDDevice, int iStatus)
 *  输入参数：ptLEDDevice-哪个LED
 *  输入参数：iStatus-LED状态, 1-亮, 0-灭
 *  输出参数：无
 *  返回值：0-成功, -1: 失败
 */
int HAL_LEDDeviceControl(struct LEDDevice *ptLEDDevice, int iStatus);

#endif /* __DRIVER_LED_H */
