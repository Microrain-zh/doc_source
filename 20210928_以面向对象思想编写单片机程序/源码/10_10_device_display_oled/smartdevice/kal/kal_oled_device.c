#include <config.h>

/**********************************************************************
 * 函数名称： KAL_OLEDDeviceInit
 * 功能描述： 内核抽象层的函数,初始化OLED硬件
 * 输入参数： ptDev-哪个显示设备
 * 输出参数： 无
 * 返 回 值： 0-成功
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/29	     V1.0	  韦东山	      创建
 ***********************************************************************/
int KAL_OLEDDeviceInit(struct DispayDevice *ptDev)
{
	/* 初始化OLED硬件 */
#if defined (CONFIG_NOOS)
	return CAL_OLEDDeviceInit(ptDev);
#elif defined (CONFIG_FREERTOS)
	return FreeRTOS_OLEDDeviceInit(ptDev);
#elif defined (CONFIG_RTTHREAD)
	return RTThread_OLEDDeviceInit(ptDev);
#endif
}

/**********************************************************************
 * 函数名称： OLEDDeviceFlush
 * 功能描述： 内核抽象层的函数, 把临时显存的数据, 刷到LCD的显存去
 * 输入参数： ptDev-哪个显示设备
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/29	     V1.0	  韦东山	      创建
 ***********************************************************************/
void KAL_OLEDDeviceFlush(struct DispayDevice *ptDev)
{
	/* 把Framebuffer g_OLEDFramebuffer的数据搬到OLED自带的显存里 */
#if defined (CONFIG_NOOS)
	CAL_OLEDDeviceFlush(ptDev);
#elif defined (CONFIG_FREERTOS)
	FreeRTOS_OLEDDeviceFlush(ptDev);
#elif defined (CONFIG_RTTHREAD)
	RTThread_OLEDDeviceFlush(ptDev);
#endif
}


