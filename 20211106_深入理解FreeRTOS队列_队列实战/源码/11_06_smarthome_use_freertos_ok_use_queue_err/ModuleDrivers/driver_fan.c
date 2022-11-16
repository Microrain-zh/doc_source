/** @file		driver_led.c
  * @author 	百问网团队
  * @version	v1.0
  * @date		2021-10-17
  * @attention
  *
  * Copyright (c) 深圳百问网科技有限公司
  * All rights reserved.
  *
  */
#include <fan_device.h>

#include "driver_fan.h"
#include "main.h"

/**********************************************************************
 * 函数名称： HAL_FanSetSpeed
 * 功能描述： 使用HAL库函数设置风扇速度
 * 输入参数： ptDevice-风扇设备的指针
 * 输入参数： iSpeed-风扇设备的速度, 
 *			  0:停止, 正数:正转,顺时针转, 负数:反转,逆时针转
 * 输出参数： 无
 * 返 回 值： 成功-0, 失败-(-1)
 * 修改日期 	  版本号 	修改人		  修改内容
 * -----------------------------------------------
 * 2021/10/18		 V1.0	  韦东山 	  创建
 ***********************************************************************/
int HAL_FanSetSpeed(struct FANDevice *ptDevice, int iSpeed)
{
	if (!ptDevice)
		return -1;

	if (iSpeed == 0)
	{
		/* INA,INB都输出0 */
		HAL_GPIO_WritePin(INA_GPIO_Port, INA_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(INB_GPIO_Port, INB_Pin, GPIO_PIN_RESET);
	}
	else if (iSpeed > 0)
	{
		/* INA输出0,INB输出1 */
		HAL_GPIO_WritePin(INA_GPIO_Port, INA_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(INB_GPIO_Port, INB_Pin, GPIO_PIN_SET);
	}
	else if (iSpeed < 0)
	{
		/* INA输出1,INB输出0 */
		HAL_GPIO_WritePin(INA_GPIO_Port, INA_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(INB_GPIO_Port, INB_Pin, GPIO_PIN_RESET);
	}
	return 0;	
}


