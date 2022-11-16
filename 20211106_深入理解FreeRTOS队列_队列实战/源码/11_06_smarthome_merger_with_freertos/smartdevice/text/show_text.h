
#ifndef __SHOW_TEXT_H
#define __SHOW_TEXT_H

#include <display_device.h>

#ifndef NULL
#define NULL (void *)0
#endif

/**********************************************************************
 * 函数名称： ShowTextInDisplayDevice
 * 功能描述： 在LCD上绘制字符串
 * 输入参数： ptDisplayDev-显示设备
 * 输入参数： iX,iY-坐标(LCD坐标系)
 * 输入参数： Str-ASCII字符串(目前不支持中文)
 * 输出参数： 无
 * 返 回 值： 0-成功, (-1)-失败
 * 修改日期        版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/12	     V1.0	  韦东山	      创建
 ***********************************************************************/
int ShowTextInDisplayDevice(PDisplayDevice ptDisplayDev, int iX, int iY, char *Str);

#endif /* __SHOW_TEXT_H */

