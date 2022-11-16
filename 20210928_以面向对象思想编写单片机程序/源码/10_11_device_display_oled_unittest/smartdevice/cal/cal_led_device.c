
#include <led_device.h>
#include <driver_led.h>


/**********************************************************************
 * 函数名称： CAL_LEDDeviceInit
 * 功能描述： 芯片抽象层的LED初始化函数, 芯片函数不一样时请修改此函数
 * 输入参数： ptLEDDevice-哪个LED设备
 * 输出参数： 无
 * 返 回 值： 0-成功
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/26	     V1.0	  韦东山	      创建
 ***********************************************************************/
int CAL_LEDDeviceInit(struct LEDDevice *ptLEDDevice)
{	
	/* 对于hal */
	/* 已经在MX_GPIO_Init初始化了引脚 */
	return 0;
}

/**********************************************************************
 * 函数名称： CAL_LEDDeviceControl
 * 功能描述： 芯片抽象层的LED控制函数, 芯片函数不一样时请修改此函数
 * 输入参数： ptLEDDevice-哪个LED设备
 * 输入参数： iStatus, 1-亮, 0-灭
 * 输出参数： 无
 * 返 回 值： 0-成功
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/26	     V1.0	  韦东山	      创建
 ***********************************************************************/
int CAL_LEDDeviceControl(struct LEDDevice *ptLEDDevice, int iStatus)
{
	/* 对于hal */
	return HAL_LEDDeviceControl(ptLEDDevice, iStatus);
}


