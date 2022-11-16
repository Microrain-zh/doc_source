#include <string.h>
#include <kal_time.h>
#include "input_system.h"
#include "input_buffer.h"

typedef void (*NetInputProcessCallback)(char c);
extern void SetNetInputProcessCallback(NetInputProcessCallback func);


enum AT_STATUS {
	INIT_STATUS,
	LEN_STATUS,
	DATA_STATUS
};

static enum AT_STATUS g_status = INIT_STATUS;


static char g_ESP8266DataBuff[INTPU_BUF_LEN];
static int g_DataBuffIndex = 0;
static int g_DataLen = 0;

/**********************************************************************
 * 函数名称： ESP8266DataProcessCallback
 * 功能描述： 网络数据数据回调函数: 得到网络数据,构造为InputEvent
 * 输入参数： c-网络数据
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/16	     V1.0	  韦东山	      创建
 ***********************************************************************/ 
static void ESP8266DataProcessCallback(char c)
{	
	/* 保存数据 */
	char *buf = g_ESP8266DataBuff;
	int i = g_DataBuffIndex;
	int m;
	InputEvent event;

	buf[i] = c;
	g_DataBuffIndex++;

	/* 解析: +IPD,n:xxxxxxxxxx */
	switch (g_status)
	{
		case INIT_STATUS:
		{
			if (buf[0] != '+')
			{
				g_DataBuffIndex = 0;
			}			
			else if (i == 4)
			{
				if (strncmp(buf, "+IPD,", 5) == 0)
				{
					g_status = LEN_STATUS;
				}
				g_DataBuffIndex = 0;
			}
			break;
		}

		case LEN_STATUS:
		{
			if (buf[i] == ':')
			{
				/* 计算长度 */
				for (m = 0; m < i; m++)
				{
					g_DataLen = g_DataLen * 10 + buf[m] - '0';
				}
				g_status = DATA_STATUS;
				g_DataBuffIndex = 0;
			}
			break;
		}

		case DATA_STATUS:
		{
			if (g_DataBuffIndex == g_DataLen)
			{
				/* 接收完数据 */
				/* 构造InputEvent并上报 */
				/* 构造InputEvent */
				event.time	= KAL_GetTime();  /* kal */
				event.eType = INPUT_EVENT_TYPE_NET;
				strncpy(event.str, buf, g_DataLen);
				event.str[g_DataLen] = '\0';
				
				/* 放入环形缓冲区 */
				PutInputEvent(&event);

				/* 恢复初始状态 */
				g_status = INIT_STATUS;
				g_DataBuffIndex = 0;
				g_DataLen = 0;
			}
			break;
		}
		
	}

	
}


/**********************************************************************
 * 函数名称： ESP8266NetInputInit
 * 功能描述： 初始化网络设备
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0-成功, 其他值-失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/16	     V1.0	  韦东山	      创建
 ***********************************************************************/ 
static int ESP8266NetInputInit(void)
{
	/* 网络硬件相关的设置在网络系统里进行 */
	
	/* 给UART中断函数提供一个回调函数 */
	SetNetInputProcessCallback(ESP8266DataProcessCallback);
	
	return 0;
}


/* 为GPIO按键构造一个InputDevice */

static InputDevice g_tNetInputDevice = {
	"esp8266",
	NULL,
	ESP8266NetInputInit,
	NULL,
};

/**********************************************************************
 * 函数名称： AddInputDeviceNet
 * 功能描述： 注册网络输入设备
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/16	     V1.0	  韦东山	      创建
 ***********************************************************************/
void AddInputDeviceNet(void)
{
	InputDeviceRegister(&g_tNetInputDevice);
}

