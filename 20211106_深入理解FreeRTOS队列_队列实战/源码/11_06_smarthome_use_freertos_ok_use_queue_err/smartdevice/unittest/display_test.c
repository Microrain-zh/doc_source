
#include <stdio.h>
#include <string.h>
#include <display_system.h>


/**********************************************************************
 * 函数名称： display_test
 * 功能描述： 显示设备单元测试函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/08	     V1.0	  韦东山	      创建
 ***********************************************************************/
void display_test(void)
{
	PDisplayDevice ptDevice;
	char *name = "OLED";
	int x, y;
	
	AddDisplayDevices();

	ptDevice = GetDisplayDevice(name);

	if (!ptDevice)
	{
		printf("Can not get DisplayDevice %s\r\n", name);
		return;
	}

	/* 1. 初始化设备 */
	ptDevice->Init(ptDevice);

	/* 2. 清除屏幕 */
	memset(ptDevice->FBBase, 0, ptDevice->iSize);

	/* 3. 画线 */
	y = ptDevice->iYres / 2;
	for (x = 0; x < ptDevice->iXres; x++)
		ptDevice->SetPixel(ptDevice, x, y, 1);

	x = ptDevice->iXres / 2;
	for (y = 0; y < ptDevice->iYres; y++)
		ptDevice->SetPixel(ptDevice, x, y, 1);


	/* 4. Flush */
	ptDevice->Flush(ptDevice);
}

