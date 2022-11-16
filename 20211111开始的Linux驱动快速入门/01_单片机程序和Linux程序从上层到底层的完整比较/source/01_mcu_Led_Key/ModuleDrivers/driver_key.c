/** @file		driver_key.c
  * @author 	百问网团队
  * @version	v1.0
  * @date		2021-8-10
  * *******************************************
  * @brief		按键驱动源文件
  * 			+
  *
  * @include	包含的头文件
  * 			+ main.h
  * 			+ driver_key.h
  * -------------------------------------------
  * *******************************************
  * -------------------------------------------
  * @attention
  *
  * Copyright (c) 深圳百问网科技有限公司
  * All rights reserved.
  *
  */

#include "driver_key.h"
#include "main.h"

/*
 *  函数名：K1_Value
 *  功能描述：获取按键KEY1的状态，按下返回0，否则返回1
 *  输入参数：无
 *  输出参数：无
 *  返回值：按键KEY1的状态
*/
uint8_t K1_Value(void)
{
    if(K1 == 0)
    {
        HAL_Delay(100);
        if(K1 == 0)
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    return 1;
}