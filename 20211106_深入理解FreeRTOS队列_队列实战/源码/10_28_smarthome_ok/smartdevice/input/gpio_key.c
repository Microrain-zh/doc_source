
#include "input_system.h"
#include "input_buffer.h"
#include <gpio_key.h>
#include <driver_key.h>

#include <kal_gpio_key.h>
#include <kal_time.h>

static unsigned int timers[2];

/**********************************************************************
 * 函数名称： GetGPIOKeyTimerVal
 * 功能描述： 获得GPIO按键的定时器的超时时间
 * 输入参数： index-哪个定时器
 * 输出参数： 无
 * 返 回 值： 定时器的超时时间
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/24	     V1.0	  韦东山	      创建
 ***********************************************************************/ 
unsigned int GetGPIOKeyTimerVal(int index)
{
	return timers[index];
}

/**********************************************************************
 * 函数名称： SetGPIOKeyTimer
 * 功能描述： 设置GPIO按键的定时器的超时时间
 * 输入参数： index-哪个定时器
 * 输入参数： val-超时时刻
 * 输出参数： 无
 * 返 回 值： 定时器的超时时间
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/24	     V1.0	  韦东山	      创建
 ***********************************************************************/ 
void SetGPIOKeyTimer(int index, unsigned int val)
{
	timers[index] = val;
}

/**********************************************************************
 * 函数名称： ProcessGPIOKeyTimer
 * 功能描述： 处理GPIO按键的定时器
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 定时器的超时时间
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/24	     V1.0	  韦东山	      创建
 ***********************************************************************/ 
void ProcessGPIOKeyTimer(void)
{
	int i;
	InputEvent event;

	for (i = 0; i < 2; i++)
	{
		/* 判断时间是否超时 */
		if ((timers[i] != 0) && (KAL_GetTick() >= timers[i]))
		{
			/* 如果超时: 上报按键 */
			/* 构造InputEvent */
			event.time  = KAL_GetTime();  /* kal */
			event.eType = INPUT_EVENT_TYPE_KEY;
			event.iKey  = (i == 0) ? K1_CODE : K2_CODE;

			/* 读取GPIO电平, GPIO为0表示按下, 此时压力是1,松开后压力是0 */
			event.iPressure = (i == 0) ? !K1_STATUS : !K2_STATUS;

			/* 放入环形缓冲区 */
			PutInputEvent(&event);

			/* 禁止定时器 */
			timers[i] = 0;
		}
	}
}

/**********************************************************************
 * 函数名称： GPIOKeyInit
 * 功能描述： 初始化GPIO按键
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0-成功, 其他值-失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/24	     V1.0	  韦东山	      创建
 ***********************************************************************/ 
static int GPIOKeyInit(void)
{
	KAL_GPIOKkeyInit();
	return 0;
}


/* 为GPIO按键构造一个InputDevice */

static InputDevice g_tKeyDevice = {
	"gpio_key",
	NULL,
	GPIOKeyInit,
	NULL,
};

/**********************************************************************
 * 函数名称： AddInputDeviceGPIOKey
 * 功能描述： 注册GPIO按键设备
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/24	     V1.0	  韦东山	      创建
 ***********************************************************************/
void AddInputDeviceGPIOKey(void)
{
	InputDeviceRegister(&g_tKeyDevice);
}


