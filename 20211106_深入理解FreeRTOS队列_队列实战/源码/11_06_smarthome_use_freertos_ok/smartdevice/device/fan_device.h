#ifndef __FAN_DEVICE_H
#define __FAN_DEVICE_H

#ifndef NULL
#define NULL (void *)0
#endif

typedef struct FANDevice {
	int speed;
	
	/* 初始化, 成功则返回0 */
	int (*Init)(struct FANDevice *ptDevice);

	/* 设置速度, 0:停止, 正数:正转,顺时针转, 负数:反转,逆时针转 */
	int (*SetSpeed)(struct FANDevice *ptDevice, int iSpeed);
	
}FANDevice, *PFANDevice;

/**********************************************************************
 * 函数名称： GetFANDevice
 * 功能描述： 获取风扇设备
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 成功-FANDevice指针, 失败-NULL
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/18	     V1.0	  韦东山	      创建
 ***********************************************************************/
PFANDevice GetFANDevice(void);


#endif /* __FAN_DEVICE_H */

