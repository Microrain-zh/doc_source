#include <kal_time.h>
#include <fan_device.h>
#include <stdio.h>


/**********************************************************************
 * 函数名称： fan_test
 * 功能描述： 设备系统风扇设备单元测试函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/18	     V1.0	  韦东山	      创建
 ***********************************************************************/
void fan_test(void)
{
	PFANDevice ptDev = GetFANDevice();

	/* 初始化设备 */
	ptDev->Init(ptDev);

	while (1)
	{
		/* 顺时针旋转 */
		ptDev->SetSpeed(ptDev, 1);
		KAL_Delay(2000);
		
		/* 停止 */
		ptDev->SetSpeed(ptDev, 0);
		KAL_Delay(2000);

		/* 逆时针旋转 */
		ptDev->SetSpeed(ptDev, -1);
		KAL_Delay(2000);		
		
		/* 停止 */
		ptDev->SetSpeed(ptDev, 0);
		KAL_Delay(2000);
	}
}

