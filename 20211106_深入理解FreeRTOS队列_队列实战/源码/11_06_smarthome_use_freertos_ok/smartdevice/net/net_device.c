
#include <stdio.h>
#include <string.h>

#include "net_device.h"


static PNetDevice g_ptNetDevices;

/**********************************************************************
 * 函数名称： NetDeviceRegister
 * 功能描述： 注册一个网卡设备
 * 输入参数： ptNetDevice-网卡设备
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/14	     V1.0	  韦东山	      创建
 ***********************************************************************/
void NetDeviceRegister(PNetDevice ptNetDevice)
{
	ptNetDevice->pNext = g_ptNetDevices;
	g_ptNetDevices = ptNetDevice;
}


/**********************************************************************
 * 函数名称： __GetNetDevice
 * 功能描述： 获得网卡设备
 * 输入参数： name-网卡设备的名称
 * 输出参数： 无
 * 返 回 值： 成功-PNetDevice, 失败-NULL
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/14	     V1.0	  韦东山	      创建
 ***********************************************************************/
PNetDevice __GetNetDevice(char *name)
{
	PNetDevice pTmp = g_ptNetDevices;
	while (pTmp)
	{
		if (strcmp(pTmp->name, name) == 0)
			return pTmp;
		else
			pTmp = pTmp->pNext;
	}
	return NULL;
}


