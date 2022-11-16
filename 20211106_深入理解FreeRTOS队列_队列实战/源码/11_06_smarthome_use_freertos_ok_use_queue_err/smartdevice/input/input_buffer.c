#include <stdio.h>
#include "input_buffer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//static InputEventBuffer g_tInputBuffer;

static QueueHandle_t g_xQueueInput;

int InitInputQueue(void)
{
	g_xQueueInput = xQueueCreate(BUFFER_SIZE, sizeof(InputEvent));
	if (g_xQueueInput == NULL)
	{
		printf("can not create queue\r\n");
		return -1;
	}
	else
	{
		return 0;
	}
	
}


/**********************************************************************
 * 函数名称： PutInputEvent
 * 功能描述： 往环形缓冲区放入数据
 * 输入参数： ptInputEvent-输入事件
 * 输出参数： 无
 * 返 回 值： 0-成功, 其他值-失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/24	     V1.0	  韦东山	      创建
 ***********************************************************************/ 
int PutInputEvent(PInputEvent ptInputEvent)
{
#if 0	
    int i = (g_tInputBuffer.pW + 1) % BUFFER_SIZE;

	/* 防御式编程 */
	if (!ptInputEvent)
		return -1;
	
    if(i != g_tInputBuffer.pR)    // 环形缓冲区没有写满
    {
        g_tInputBuffer.buffer[g_tInputBuffer.pW] = *ptInputEvent;
        g_tInputBuffer.pW = i;
		return 0;
    }
	return -1;
#else
	/* 写队列 */
	xQueueSendFromISR(g_xQueueInput, ptInputEvent, NULL);
	return 0;
#endif
}

/**********************************************************************
 * 函数名称： GetInputEvent
 * 功能描述： 从环形缓冲区读取数据
 * 输入参数： 无
 * 输出参数： ptInputEvent-用来保存输入事件
 * 返 回 值： 0-成功, 其他值-失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/09/24	     V1.0	  韦东山	      创建
 ***********************************************************************/ 
int GetInputEvent(PInputEvent ptInputEvent)
{
#if 0	
	/* 防御式编程 */
	if (!ptInputEvent)
		return -1;

	/* 环形缓冲区空, 则返回-1 */
    if(g_tInputBuffer.pR == g_tInputBuffer.pW)
    {
        return -1;
    }
    else
    {
        *ptInputEvent = g_tInputBuffer.buffer[g_tInputBuffer.pR];
        g_tInputBuffer.pR = (g_tInputBuffer.pR + 1) % BUFFER_SIZE;
        return 0;
    }
#else
	/* 读队列 */
	xQueueReceive(g_xQueueInput, ptInputEvent, portMAX_DELAY);
	return 0;
#endif
}

