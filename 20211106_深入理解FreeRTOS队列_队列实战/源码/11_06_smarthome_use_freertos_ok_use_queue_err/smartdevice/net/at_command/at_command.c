#include <config.h>
#include <stdio.h>
#include <string.h>
#include <uart_dev.h>
#include <kal_time.h>

#include "net_device.h"

#define AT_INTERFACE_TIMEOUT 10

static PATInterfaceDevice g_ptATIntfDev;

/**********************************************************************
 * 函数名称： isReturnOK
 * 功能描述： 判断字符串里是否有"OK\r\n"
 * 输入参数： buf-字符串
 * 输入参数： len-数据长度
 * 输出参数： 无
 * 返 回 值： 1-含有, 0-不含有
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/15	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int isReturnOK(char *buf, int len)
{
	/* 判断字符串里有没有: "OK\r\n" */
	buf[len] = '\0';
	return strstr(buf, "OK\r\n") ? 1 : 0;
}

/**********************************************************************
 * 函数名称： isReturnERROR
 * 功能描述： 判断字符串里是否有"ERROR\r\n"
 * 输入参数： buf-字符串
 * 输入参数： len-数据长度
 * 输出参数： 无
 * 返 回 值： 1-含有, 0-不含有
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/15	     V1.0	  韦东山	      创建
 ***********************************************************************/
static int isReturnERROR(char *buf, int len)
{
	/* 判断字符串里有没有: "ERROR\r\n" */
	buf[len] = '\0';
	return strstr(buf, "ERROR\r\n") ? 1 : 0;
}

/**********************************************************************
 * 函数名称： ATInterfaceInit
 * 功能描述： 初始化硬件接口
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 0-成功
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/15	     V1.0	  韦东山	      创建
 ***********************************************************************/
int ATInterfaceInit(void)
{
	/* 获得某个设备 */
	g_ptATIntfDev = GetATInterfaceDevice();
	g_ptATIntfDev->Init(g_ptATIntfDev);
	return 0;
}

/**********************************************************************
 * 函数名称： ATCommandSend
 * 功能描述： 发送AT命令,并等待、解析返回结果
 * 输入参数： cmd-AT命令
 * 输入参数： iTimeoutMS-超时时间
 * 输出参数： piLen-用来数据长度
 * 返 回 值： 0-成功, (-1)-错误, (-2)-超时
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/15	     V1.0	  韦东山	      创建
 ***********************************************************************/
int ATCommandSend(char *cmd, int iTimeoutMS)
{
	char buf[100];
	int i = 0;
	int pre;
	int now;

	debug_printf("Send ATCommand: %s\r\n", cmd);

	g_ptATIntfDev->InvalidRecvBuf(g_ptATIntfDev);
	
	g_ptATIntfDev->Write(g_ptATIntfDev, cmd, strlen(cmd));
	g_ptATIntfDev->Write(g_ptATIntfDev, "\r\n", 2);

	/* 等待返回的数据, 判断结果 */
	/* ReadByte返回的条件:
	 *  读到了1个字节
	 *  没有数据也立刻返回
	 */
	pre = KAL_GetTime();
	while (iTimeoutMS)
	{
		if (g_ptATIntfDev->ReadByte(g_ptATIntfDev, &buf[i]) == 0)
		{
			debug_printf("%c", buf[i]);
			/* 有没有碰到回车换行"\r\n" */
			if ((i != 0) && (buf[i-1] == '\r') && (buf[i] == '\n'))
			{
				if (isReturnOK(buf, i+1))
				{
					debug_printf(" OK\r\n");
					return 0;
				}
				else if(isReturnERROR(buf, i+1))
				{
					debug_printf(" ERROR\r\n");
					return -1;
				}
				else
				{
					i = 0;
				}
			}
			i++;
		}
		
		now = KAL_GetTime();
		if (now > pre)
		{
			iTimeoutMS--;
			pre = now;
		}
		
	}

	debug_printf(" timeout\r\n");
	return -2;
}

/**********************************************************************
 * 函数名称： ATCommandSendAndRecv
 * 功能描述： 发送AT命令,并返回接收到的数据
 * 输入参数： cmd-AT命令
 * 输入参数： iTimeoutMS-超时时间
 * 输出参数： pcRecvBuf-用来数据
 * 返 回 值： 0-成功, (-1)-错误, (-2)-超时
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/15	     V1.0	  韦东山	      创建
 ***********************************************************************/
int ATCommandSendAndRecv(char *cmd, char *pcRecvBuf, int iTimeoutMS)
{
	char *buf = pcRecvBuf;
	int i = 0;
	int pre;
	int now;

	g_ptATIntfDev->InvalidRecvBuf(g_ptATIntfDev);
	
	g_ptATIntfDev->Write(g_ptATIntfDev, cmd, strlen(cmd));
	g_ptATIntfDev->Write(g_ptATIntfDev, "\r\n", 2);

	/* 等待返回的数据, 判断结果 */
	/* ReadByte返回的条件:
	 *	读到了1个字节
	 *	没有数据也立刻返回
	 */
	pre = KAL_GetTime();
	while (iTimeoutMS)
	{
		if (g_ptATIntfDev->ReadByte(g_ptATIntfDev, &buf[i]) == 0)
		{
			/* 有没有碰到回车换行"\r\n" */
			if ((i != 0) && (buf[i-1] == '\r') && (buf[i] == '\n'))
			{
				if (isReturnOK(buf, i+1))
				{
					buf[i+1] = '\0';
					return 0;
				}
				else if(isReturnERROR(buf, i+1))
				{
					buf[i+1] = '\0';
					return -1;
				}
			}
			i++;
		}
		
		now = KAL_GetTime();
		if (now > pre)
		{
			iTimeoutMS--;
			pre = now;
		}
		
	}

	return -2;
}



/**********************************************************************
 * 函数名称： ATDataRecv
 * 功能描述： 接收网络数据
 * 输入参数： ptDev-网卡设备
 * 输入参数： Data-数据buffer
 * 输入参数： iTimeoutMS-超时时间
 * 输出参数： piLen-用来数据长度
 * 返 回 值： 0-成功, (-1)-错误, (-2)-超时
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/15	     V1.0	  韦东山	      创建
 ***********************************************************************/
int ATDataRecv(unsigned char *Data, int *piLen, int iTimeoutMS)
{
	/* 数据格式: +IPD,2:ab */
	enum AT_STATUS {
		INIT_STATUS,
		LEN_STATUS,
		DATA_STATUS
	};

	enum AT_STATUS status = INIT_STATUS;

	char buf[100];
	int i = 0;
	int pre;
	int now;
	int len = 0;
	int m;

	pre = KAL_GetTime();
	while (iTimeoutMS)
	{
		if (g_ptATIntfDev->ReadByte(g_ptATIntfDev, &buf[i]) == 0)
		{
			switch (status)
			{
				case INIT_STATUS:
				{
					buf[i+1] = '\0';
					if (strstr(buf, "+IPD,"))
					{
						status = LEN_STATUS;
						i = 0;
					}
					else
					{
						i++;
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
							len = len * 10 + buf[m] - '0';
						}
						status = DATA_STATUS;
						i = 0;
					}
					else
					{
						i++;
					}
					break;
				}

				case DATA_STATUS:
				{
					Data[i] = buf[i];
					i++;
					if (i == len)
					{
						if (piLen)
						{
							*piLen = len;
						}
						return 0;
					}
					break;
				}

			}
		}
		
		now = KAL_GetTime();
		if (now > pre)
		{
			iTimeoutMS--;
			pre = now;
		}
		
	}

	return -2;
	
	
}


