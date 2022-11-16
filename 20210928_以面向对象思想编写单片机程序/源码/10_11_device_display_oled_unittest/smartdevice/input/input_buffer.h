#ifndef __INPUT_BUFFER_H
#define __INPUT_BUFFER_H

#include "input_system.h"

#define BUFFER_SIZE 10        /* 环形缓冲区的大小 */
typedef struct
{
    InputEvent buffer[BUFFER_SIZE];  /* 缓冲区空间 */
    volatile unsigned int pW;           /* 写地址 */
    volatile unsigned int pR;           /* 读地址 */
} InputEventBuffer;

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
int GetInputEvent(PInputEvent ptInputEvent);

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
int PutInputEvent(PInputEvent ptInputEvent);

#endif /* __INPUT_BUFFER_H */

