
#include <led_device.h>
#include <led_device.h>
#include <cal_led_device.h>

/**********************************************************************
 * 函数名称： KAL_LEDDeviceInit
 * 功能描述： 内核抽象层的LED初始化函数, 内核不一样时请修改此函数
 * 输入参数： ptLEDDevice-哪个LED设备
 * 输出参数： 无
 * 返 回 值： 0-成功
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/26	     V1.0	  韦东山	      创建
 ***********************************************************************/
int KAL_LEDDeviceInit(struct LEDDevice *ptLEDDevice)
{	
	/* 对于裸机/FreeRTOS */
	return CAL_LEDDeviceInit(ptLEDDevice);

	/* 对于RT-Thread */

	/* 对于Linux */
}

/**********************************************************************
 * 函数名称： KAL_LEDDeviceControl
 * 功能描述： 内核抽象层的LED控制函数, 内核不一样时请修改此函数
 * 输入参数： ptLEDDevice-哪个LED设备
 * 输入参数： iStatus, 1-亮, 0-灭
 * 输出参数： 无
 * 返 回 值： 0-成功
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/26	     V1.0	  韦东山	      创建
 ***********************************************************************/
int KAL_LEDDeviceControl(struct LEDDevice *ptLEDDevice, int iStatus)
{
	/* 对于裸机/FreeRTOS */
	return CAL_LEDDeviceControl(ptLEDDevice, iStatus);

	/* 对于RT-Thread */

	/* 对于Linux */
}


