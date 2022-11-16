#include <config.h>
#include <display_device.h>

#if defined (CONFIG_SUPPORT_HAL)
#include "driver_i2c.h"
#include "driver_oled.h"
#endif

/**********************************************************************
 * 函数名称： CAL_OLEDDeviceInit
 * 功能描述： 芯片抽象层的函数,初始化OLED硬件
 * 输入参数： ptDev-哪个显示设备
 * 输出参数： 无
 * 返 回 值： 0-成功
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/29	     V1.0	  韦东山	      创建
 ***********************************************************************/
int CAL_OLEDDeviceInit(struct DisplayDevice *ptDev)
{
	/* 初始化OLED硬件 */
#if defined (CONFIG_SUPPORT_HAL)
    // 1. 重新初始化I2C的引脚
    I2C_GPIO_ReInit();
    // 2. 初始化OLED  
    OLED_Init();
	return 0;
#elif defined (CONFIG_SUPPORT_NOHAL)
	return NOHAL_OLEDDeviceInit(ptDev);
#endif
}

/**********************************************************************
 * 函数名称： CAL_OLEDDeviceFlush
 * 功能描述： 芯片抽象层的函数, 把临时显存的数据, 刷到LCD的显存去
 * 输入参数： ptDev-哪个显示设备
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/29	     V1.0	  韦东山	      创建
 ***********************************************************************/
void CAL_OLEDDeviceFlush(struct DisplayDevice *ptDev)
{
	/* 把Framebuffer g_OLEDFramebuffer的数据搬到OLED自带的显存里 */
#if defined (CONFIG_SUPPORT_HAL)
	OLED_Copy(ptDev->FBBase);
#elif defined (CONFIG_SUPPORT_NOHAL)
	NOHAL_OLEDDeviceFlush(ptDev);
#endif
}


