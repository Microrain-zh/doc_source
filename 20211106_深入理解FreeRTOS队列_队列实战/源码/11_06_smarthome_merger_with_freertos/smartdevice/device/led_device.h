#ifndef __LED_DEVICE_H
#define __LED_DEVICE_H

#ifndef NULL
#define NULL (void *)0
#endif

#define LED_WHITE 	0
#define LED_BLUE 	1
#define LED_GREEN 	2

typedef struct LEDDevice {
	int which;

	int iStatus;  /* 当前状态 */

	/* 初始化LED设备, 成功则返回0 */
	int (*Init)(struct LEDDevice *ptLEDDevice);

	/* 控制LED设备, iStatus取值: 1-亮,0-灭,2-反转 */
	int (*Control)(struct LEDDevice *ptLEDDevice, int iStatus);

	/* 未实现 */
	void (*SetColor)(struct LEDDevice *ptLEDDevice, int iColor);

	/* 未实现 */
	void (*SetBrightness)(struct LEDDevice *ptLEDDevice, int iBrightness);
}LEDDevice, *PLEDDevice;

/**********************************************************************
 * 函数名称： GetLEDDevice
 * 功能描述： 获取LED设备
 * 输入参数： which-哪个LED设备
 *            取值有: LED_WHITE,LED_BLUE或LED_GREEN
 * 输出参数： 无
 * 返 回 值： 成功-LEDDevice指针, 失败-NULL
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/26	     V1.0	  韦东山	      创建
 ***********************************************************************/
PLEDDevice GetLEDDevice(int which);


#endif /* __LED_DEVICE_H */

