#ifndef __MESSAGE_H
#define __MESSAGE_H

#ifndef NULL
#define NULL (void *)0
#endif

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
int ConvertInputEventToJson(PInputEvent ptEvent, char *pcJson);


#endif /* __MESSAGE_H */

