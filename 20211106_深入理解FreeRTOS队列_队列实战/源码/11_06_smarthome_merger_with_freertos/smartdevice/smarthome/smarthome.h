#ifndef __SMART_HOME_H
#define __SMART_HOME_H

#ifndef NULL
#define NULL (void *)0
#endif

/* 你做实验时需要修改这2项: WIFI热点的名称和密码 */
#define SSID        "Programmers"
#define PASSWORD    "100asktech"

#define LOCAL_PORT  1234
#define VERSION  "V1.0 From 100ASK"


/**********************************************************************
 * 函数名称： SmartHomeTask
 * 功能描述： 智能家居任务函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/16	     V1.0	  韦东山	      创建
 ***********************************************************************/
void SmartHomeTask(void);


#endif /* __SMART_HOME_H */

