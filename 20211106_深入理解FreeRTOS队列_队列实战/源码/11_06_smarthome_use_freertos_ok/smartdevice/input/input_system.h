#ifndef __INPUT_SYSTEM_H
#define __INPUT_SYSTEM_H

#ifndef NULL
#define NULL (void *)0
#endif

#define TIME_T int
#define INTPU_BUF_LEN 50

typedef enum
{    
	INPUT_EVENT_TYPE_KEY,
	INPUT_EVENT_TYPE_TOUCH,
	INPUT_EVENT_TYPE_NET,
	INPUT_EVENT_TYPE_STDIO
}INPUT_EVENT_TYPE;

typedef struct InputEvent {
	TIME_T time;
	INPUT_EVENT_TYPE eType;
	int iX;
	int iY;
	int iKey;
	int iPressure;
	char str[INTPU_BUF_LEN];
}InputEvent, *PInputEvent;

typedef struct InputDevice {
	char *name;
	int (*GetInputEvent)(PInputEvent ptInputEvent);
	int (*DeviceInit)(void);
	int (*DeviceExit)(void);
	struct InputDevice *pNext;
}InputDevice, *PInputDevice;


/**********************************************************************
 * 函数名称： AddInputDevices
 * 功能描述： 注册多个输入设备
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/24	     V1.0	  韦东山	      创建
 ***********************************************************************/
void AddInputDevices(void);


/**********************************************************************
 * 函数名称： InitInputDevices
 * 功能描述： 初始化所有的输入设备
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/24	     V1.0	  韦东山	      创建
 ***********************************************************************/
void InitInputDevices(void);

/**********************************************************************
 * 函数名称： InputDeviceRegister
 * 功能描述： 注册一个输入设备
 * 输入参数： ptInputDevice-输入设备
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/24	     V1.0	  韦东山	      创建
 ***********************************************************************/
void InputDeviceRegister(PInputDevice ptInputDevice);


#endif /* __INPUT_SYSTEM_H */



