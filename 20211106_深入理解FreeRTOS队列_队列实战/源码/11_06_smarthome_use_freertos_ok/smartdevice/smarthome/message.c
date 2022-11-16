#include <stdio.h>
#include <string.h>
#include <input_buffer.h>
#include <input_system.h>
#include <led_device.h>
#include <fan_device.h>
#include <display_system.h>
#include <font_system.h>
#include <show_text.h>
#include <net_system.h>
#include <gpio_key.h>


/**********************************************************************
 * 函数名称： ConvertInputEventToJson
 * 功能描述： 把输入事件InputEvent转换为Json格式的参数
 * 输入参数： ptEvent-输入事件的指针
 * 输出参数： pcJson-Json格式的参数
 * 返 回 值： 0-成功, (-1)-失败
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/16	     V1.0	  韦东山	      创建
 ***********************************************************************/
int ConvertInputEventToJson(PInputEvent ptEvent, char *pcJson)
{
	if (ptEvent->eType == INPUT_EVENT_TYPE_NET)
	{
		strcpy(pcJson, ptEvent->str);
		return 0;
	}
	else if (ptEvent->eType == INPUT_EVENT_TYPE_KEY)
	{
		if (ptEvent->iPressure)
		{
			return -1;
		}

		/* KEY1-->lamp1 : {"dev":"lamp1","status":"2"} */
		/* KEY2-->lamp2 : {"dev":"lamp2","status":"2"} */
		if (ptEvent->iKey == K1_CODE)
			strcpy(pcJson, "{\"dev\":\"lamp1\",\"status\":\"2\"}");
		else if (ptEvent->iKey == K2_CODE)
			strcpy(pcJson, "{\"dev\":\"lamp2\",\"status\":\"2\"}");
		else
			return -1;
		return 0;
	}

	return -1;
}

