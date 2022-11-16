/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：driver_i2c.c
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2020.6.6      v01         百问科技      创建文件
 *--------------------------------------------------
*/
#include "driver_i2c.h"

/*
 *  函数名：I2C_Delay
 *  功能描述：使用软件方式延时
 *  输入参数：t-->延时事件
 *  输出参数：无
 *  返回值：无
*/
void I2C_Delay(uint32_t t)
{
    volatile uint32_t tmp = t;
    while(tmp--);
}

/*
 *  函数名：I2C_GPIO_ReInit
 *  功能描述：将模拟I2C的SCL和SDA引脚进行重新初始化
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
void I2C_GPIO_ReInit(void)
{
    /* 1. 使用结构体定义硬件GPIO对象 */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* 2. 将SCL和SDA的GPIO寄存器的值恢复为复位上电值 */
    HAL_GPIO_DeInit(SCL_PORT, SCL_PIN);
    HAL_GPIO_DeInit(SDA_PORT, SDA_PIN);
    
    /* 3. 使能SCL和SDA的GPIO的时钟，因为他们都是GPIOF组的，所以这里只使能GPIOF的时钟 */
    __HAL_RCC_GPIOF_CLK_ENABLE();
    
    /* 4. 设置GPIO的模式为开漏输出模式，响应速度设置为快速响应 */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        
    /* 5. 选择要设置的GPIO引脚 */
    GPIO_InitStruct.Pin = SCL_PIN;
    
    /* 6. 调用init函数初始化GPIO */
    HAL_GPIO_Init(SCL_PORT, &GPIO_InitStruct);
    
    GPIO_InitStruct.Pin = SDA_PIN;
    HAL_GPIO_Init(SDA_PORT, &GPIO_InitStruct);
}

/*
 *  函数名：I2C_Start
 *  功能描述：模拟发出I2C的开始信号-->在SCL的高电平下，SDA的电平由高到低变化
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
void I2C_Start(void)
{
    SCL_H;
    SDA_H;
    I2C_Delay(100);
    SDA_L;
    I2C_Delay(100);
}

/*
 *  函数名：I2C_Stop
 *  功能描述：模拟发出I2C的停止信号-->在SCL的高电平下，SDA的电平由低到高变化
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
void I2C_Stop(void)
{
    SDA_L;
    SCL_H;
    I2C_Delay(100);
    SDA_H;
    I2C_Delay(100);
}

/*
 *  函数名：I2C_GetAck
 *  功能描述：模拟I2C获取从设备的响应信号-->在SCL的高电平下，从设备将SDA拉低
 *  输入参数：无
 *  输出参数：无
 *  返回值：成功得到响应返回0，否则返回-1
*/
int I2C_GetAck(void)
{
    int i = 0;
    SCL_L;
    SDA_H;
    I2C_Delay(100);
    SCL_H;
    while(SDA_IN != 0)
    {
        i++;
        if(i == 100)
        {
            SCL_L;
            return -1;
        }
        I2C_Delay(10);
    }
    SCL_L;
    return 0;
}

/*
 *  函数名：I2C_ACK
 *  功能描述：模拟I2C发出一个应答信号，在第九个时钟将SDA拉低
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
void I2C_ACK(void)
{
    SCL_L;
    SDA_L;
    I2C_Delay(100);
    SCL_H;
    I2C_Delay(100);
}

/*
 *  函数名：I2C_NACK
 *  功能描述：模拟I2C发出一个非应答信号，在第九个时钟将SDA拉高
 *  输入参数：无
 *  输出参数：无
 *  返回值：无
*/
void I2C_NACK(void)
{
    SCL_L;
    SDA_H;
    I2C_Delay(100);
    SCL_H;
    I2C_Delay(100);
}

/*
 *  函数名：I2C_WriteByte
 *  功能描述：模拟I2C发出一个字节的数据
 *  输入参数：data-->要发送出去的数据，范围0~255
 *  输出参数：无
 *  返回值：无
*/
void I2C_WriteByte(uint8_t data)
{
    uint8_t i = 0;
    for(i=0; i<8; i++)
    {
        SCL_L;
        I2C_Delay(100);
        if(data & 0x80)
        {
            SDA_H;
        }
        else
        {
            SDA_L;
        }
        data <<= 1; // 发出1bit数据后，要更新数据，将data的次高位移位到最高位
        SCL_H;
        I2C_Delay(100);
    }
    I2C_GetAck();
}

/*
 *  函数名：I2C_ReadByte
 *  返回值：返回读取到的数据
 *  输入参数：ack-->根据此参数判断在读到一个字节之后是否发出应答信号
 *  输出参数：无
*/
uint8_t I2C_ReadByte(uint8_t ack)
{
    uint8_t i = 0;
    uint8_t data = 0;
    
    SDA_H;
    for(i=0; i<8; i++)
    {
        SCL_L;
        I2C_Delay(100);
        SCL_H;
        I2C_Delay(100);
        data <<= 1; // 更新数据前，要将上一次数据左移1位用来保存接下来的这一位数据
        if(SDA_IN == 1)    
        {                 
            data++;
        }
        else
        {
            data = data;
        }
    }
    
    // 根据ack决定是否发出应答
    if(ack == 0)
    {
        I2C_ACK();
    }
    else if(ack == 1)
    {
        I2C_NACK();
    }
    
    return data;
}

