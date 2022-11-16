
#include "display_device.h"

/* 为OLED构造一个DisplayDevice结构体 */

/* 128*64 bits, 1024 bytes */
static unsigned char g_OLEDFramebuffer[1024];

/**********************************************************************
 * 函数名称： OLEDDeviceInit
 * 功能描述： 初始化OLED硬件
 * 输入参数： ptDev-哪个显示设备
 * 输出参数： 无
 * 返 回 值： 0-成功
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/29	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int OLEDDeviceInit(struct DispayDevice *ptDev)
{
	/* 初始化OLED硬件 */
	return KAL_OLEDDeviceInit(ptDev);
}

/**********************************************************************
 * 函数名称： OLEDDeviceFlush
 * 功能描述： 把临时显存的数据, 刷到LCD的显存去
 * 输入参数： ptDev-哪个显示设备
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/29	     V1.0	  韦东山	      创建
 ***********************************************************************/
static void OLEDDeviceFlush(struct DispayDevice *ptDev)
{
	/* 把Framebuffer g_OLEDFramebuffer的数据搬到OLED自带的显存里 */
	KAL_OLEDDeviceFlush(ptDev);
}

/**********************************************************************
 * 函数名称： OLEDDeviceSetPixel
 * 功能描述： 在显存中设置(iX,iY)像素的颜色
 * 输入参数： ptDev-哪个显示设备
 * 输入参数： iX-X坐标
 * 输入参数： iY-Y坐标
 * 输入参数： dwColor-颜色
 * 输出参数： 无
 * 返 回 值： 0-成功
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/29	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int OLEDDeviceSetPixel(struct DispayDevice *ptDev, int iX, int iY, unsigned int dwColor) /* 0x00RRGGBB */
{
	unsigned char *buf = ptDev->FBBase;
	int page;
	unsigned char *byte;
	int bit;
	
	if (iX >= ptDev->iXres || iY >= ptDev->iYres)
		return -1;

	page = iY / 8;
	byte = buf + page * 128 + iX;
	bit = iY % 8;

	if (dwColor)
		*byte |= (1<<bit);
	else
		*byte &= ~(1<<bit);

	return 0;
}

static DispayDevice g_tOLEDDevice = {
	"OLED",
	g_OLEDFramebuffer,
	128,
	64,
	1,
	OLEDDeviceInit,
	OLEDDeviceFlush,
	OLEDDeviceSetPixel
};

