
#include <kal_led_device.h>
#include "led_device.h"

/**********************************************************************
 * 函数名称： KAL_LEDDeviceInit
 * 功能描述： 设备层的LED初始化函数
 * 输入参数： ptLEDDevice-哪个LED设备
 * 输出参数： 无
 * 返 回 值： 0-成功
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/26	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int LEDDeviceInit(struct LEDDevice *ptLEDDevice)
{
	return KAL_LEDDeviceInit(ptLEDDevice);
}

/**********************************************************************
 * 函数名称： LEDDeviceControl
 * 功能描述： 设备层的LED控制函数
 * 输入参数： ptLEDDevice-哪个LED设备
 * 输入参数： iStatus, 1-亮, 0-灭
 * 输出参数： 无
 * 返 回 值： 0-成功
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/26	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int LEDDeviceControl(struct LEDDevice *ptLEDDevice, int iStatus)
{
	return KAL_LEDDeviceControl(ptLEDDevice, iStatus);
}


static LEDDevice g_tLEDDevices[] = {
	{LED_WHITE, LEDDeviceInit, LEDDeviceControl},
	{LED_BLUE,  LEDDeviceInit, LEDDeviceControl},
	{LED_GREEN, LEDDeviceInit, LEDDeviceControl},
};

/**********************************************************************
 * 函数名称： GetLEDDevice
 * 功能描述： 获取LED设备
 * 输入参数： which-哪个LED设备
 *            取值有: LED_WHITE,LED_BLUE或LED_GREEN
 * 输出参数： 无
 * 返 回 值： 成功-LEDDevice指针, 失败-NULL
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/26	     V1.0	  韦东山	      创建
 ***********************************************************************/
PLEDDevice GetLEDDevice(int which)
{
	if (which >= LED_WHITE && which <= LED_GREEN)
		return &g_tLEDDevices[which];
	else
		return NULL;
}

