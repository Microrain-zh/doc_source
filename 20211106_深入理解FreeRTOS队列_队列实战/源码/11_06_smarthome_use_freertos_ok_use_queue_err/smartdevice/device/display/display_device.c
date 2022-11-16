
#include <stdio.h>
#include <string.h>

#include "display_device.h"


static PDisplayDevice g_ptDisplayDevices;

/**********************************************************************
 * 函数名称： DisplayDeviceRegister
 * 功能描述： 注册一个显示设备
 * 输入参数： ptDisplayDevice-显示设备
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/24	     V1.0	  韦东山	      创建
 ***********************************************************************/
void DisplayDeviceRegister(PDisplayDevice ptDisplayDevice)
{
	ptDisplayDevice->pNext = g_ptDisplayDevices;
	g_ptDisplayDevices = ptDisplayDevice;
}


/**********************************************************************
 * 函数名称： __GetDisplayDevice
 * 功能描述： 获得显示设备
 * 输入参数： name-显示设备的名称
 * 输出参数： 无
 * 返 回 值： 成功-PDisplayDevice, 失败-NULL
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
PDisplayDevice __GetDisplayDevice(char *name)
{
	PDisplayDevice pTmp = g_ptDisplayDevices;
	while (pTmp)
	{
		if (strcmp(pTmp->name, name) == 0)
			return pTmp;
		else
			pTmp = pTmp->pNext;
	}
	return NULL;
}


