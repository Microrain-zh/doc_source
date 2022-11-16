/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_i2c.h
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2020.6.6      v01         百问科技      创建文件
 *--------------------------------------------------
*/


#ifndef __DRIVER_I2C_H
#define __DRIVER_I2C_H

#include "stm32f1xx_hal.h"

/*********************
 * 引脚宏定义
**********************/
#define SCL_PIN         GPIO_PIN_10
#define SDA_PIN         GPIO_PIN_11

#define SCL_PORT        GPIOF
#define SDA_PORT        GPIOF

/*********************
 * 函数宏定义
**********************/
#define SCL_H           HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, 1)
#define SCL_L           HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, 0)

#define SDA_H           HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, 1)
#define SDA_L           HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, 0)
#define SDA_IN          HAL_GPIO_ReadPin(SDA_PORT, SDA_PIN)

/*
 *  函数名：I2C_GPIO_ReInit
 *  功能描述：将模拟I2C的SCL和SDA引脚进行重新初始化
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
extern void I2C_GPIO_ReInit(void);

/*
 *  函数名：I2C_Start
 *  功能描述：模拟发出I2C的开始信号-->在SCL的高电平下，SDA的电平由高到低变化
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
extern void I2C_Start(void);

/*
 *  函数名：I2C_Stop
 *  功能描述：模拟发出I2C的停止信号-->在SCL的高电平下，SDA的电平由低到高变化
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
extern void I2C_Stop(void);

/*
 *  函数名：I2C_GetAck
 *  功能描述：模拟I2C获取从设备的响应信号-->在SCL的高电平下，从设备将SDA拉低
 *  输入参数：无
 *  输出参数：无
 *  返回值：成功得到响应返回0，否则返回-1
*/
extern int I2C_GetAck(void);

/*
 *  函数名：I2C_ACK
 *  功能描述：模拟I2C发出一个应答信号，在第九个时钟将SDA拉低
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
extern void I2C_ACK(void);

/*
 *  函数名：I2C_NACK
 *  功能描述：模拟I2C发出一个非应答信号，在第九个时钟将SDA拉高
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
extern void I2C_NACK(void);

/*
 *  函数名：I2C_WriteByte
 *  功能描述：模拟I2C发出一个字节的数据
 *  输入参数：data-->要发送出去的数据，范围0~255
 *  输出参数：无
 *  返回值：无
*/
extern void I2C_WriteByte(uint8_t data);

/*
 *  函数名：I2C_ReadByte
 *  返回值：返回读取到的数据
 *  输入参数：ack-->根据此参数判断在读到一个字节之后是否发出应答信号
 *  输出参数：无
*/
extern uint8_t I2C_ReadByte(uint8_t ack);

#endif /* __DRIVER_I2C_H */