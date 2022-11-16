
#include <fan_device.h>
#include <cal_fan_device.h>

/**********************************************************************
 * 函数名称： KAL_FanSetSpeed
 * 功能描述： 内核抽象层的设置风扇速度
 * 输入参数： ptDevice-风扇设备的指针
 * 输入参数： iSpeed-风扇设备的速度, 
 *			  0:停止, 正数:正转,顺时针转, 负数:反转,逆时针转
 * 输出参数： 无
 * 返 回 值： 成功-0, 失败-(-1)
 * 修改日期 	  版本号 	修改人		  修改内容
 * -----------------------------------------------
 * 2021/10/18		 V1.0	  韦东山 	  创建
 ***********************************************************************/
int KAL_FanSetSpeed(struct FANDevice *ptDevice, int iSpeed)
{	
	/* 对于裸机/FreeRTOS */
	return CAL_FanSetSpeed(ptDevice, iSpeed);

	/* 对于RT-Thread */

	/* 对于Linux */
}

