#include <stdio.h>
#include <string.h>
#include <display_system.h>
#include <font_system.h>
#include <show_text.h>


/**********************************************************************
 * 函数名称： text_test
 * 功能描述： 文本系统单元测试函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/13	     V1.0	  韦东山	      创建
 ***********************************************************************/
void text_test(void)
{
	char *str = "www.100ask.net";
	PDisplayDevice ptDevice;
	char *name = "OLED";

	/* 添加字库 */
	AddFontLibs();

	/* 选择默认字库 */
	SetDefaultFontLib("ascii");

	/* 初始化默认字库 */
	InitDefaultFontLib();

	/* 添加显示设备 */
	AddDisplayDevices();

	/* 获得指定的显示设备 */
	ptDevice = GetDisplayDevice(name);

	if (!ptDevice)
	{
		printf("Can not get DisplayDevice %s\r\n", name);
		return;
	}

	/* 初始化设备 */
	ptDevice->Init(ptDevice);

	/* 清除屏幕 */
	memset(ptDevice->FBBase, 0, ptDevice->iSize);

	/* 在屏幕上显示文字 */
	ShowTextInDisplayDevice(ptDevice, 16, 16, str);
}



