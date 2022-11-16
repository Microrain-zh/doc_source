#ifndef __UART_DEV_H
#define __UART_DEV_H

#include <ring_buffer.h>

#define ATInterfaceDevice  UARTDevice
#define PATInterfaceDevice PUARTDevice

typedef struct UARTDevice {
	char *name;
	int (*Init)(struct UARTDevice *ptDev);
	void (*InvalidRecvBuf)(struct UARTDevice *ptDev);
	int (*Write)(struct UARTDevice *ptDev, char *Data, int iLen);
	int (*ReadByte)(struct UARTDevice *ptDev, char *Data);
}UARTDevice, *PUARTDevice;


/**********************************************************************
 * 函数名称： GetUART3RingBuffer
 * 功能描述： 返回UART3的环形缓冲区
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 环形缓冲区的地址
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/15	     V1.0	  韦东山	      创建
 ***********************************************************************/
ring_buffer *GetUART3RingBuffer(void);

/**********************************************************************
 * 函数名称： GetATInterfaceDevice
 * 功能描述： 获得UART3设备
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： UART3设备的结构体指针
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/15	     V1.0	  韦东山	      创建
 ***********************************************************************/
PATInterfaceDevice GetATInterfaceDevice(void);

#endif /* __UART_DEV_H */

