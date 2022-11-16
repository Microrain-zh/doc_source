#include <stdio.h>
#include <string.h>
#include <kal_time.h>
#include <net_system.h>
#include <input_system.h>
#include <input_buffer.h>


/**********************************************************************
 * 函数名称： net_test
 * 功能描述： 网络系统单元测试函数
 * 输入参数： 无
 * 输出参数： 无
 * 返 回 值： 无
 * 修改日期       版本号     修改人	      修改内容
 * -----------------------------------------------
 * 2021/10/15	     V1.0	  韦东山	      创建
 ***********************************************************************/
void net_test(void)
{
	PNetDevice pNetDev;
	char ip[20];
	int port = 1234;
	InputEvent event;

	/* 等待2S, 等待ESP8266启动完毕 */
	KAL_Delay(2000);


	/* 添加网卡 */
	AddNetDevices();

	/* 获得ESP8266网卡 */
	pNetDev = GetNetDevice("esp8266");

	/* 初始化网卡 */
	pNetDev->Init(pNetDev);

	while (1)
	{
		/* 连接WIFI热点: 你做实验时需要修改这里: wifi热点名,密码 */
		if (pNetDev->Connect(pNetDev, "Programmers", "100asktech") == 0)
		{
			printf("Connect WIFI ok\r\n");
			break;
		}
		else
		{
			printf("Connect WIFI err\r\n");
			KAL_Delay(1000);
		}
	}
	/* 获得单板IP */
	if (pNetDev->GetInfo(pNetDev, ip) == 0)
	{
		printf("Board IP = %s, port = %d\r\n", ip, port);
	}
	else
	{
		printf("GetInfo err\r\n");
		return;
	}
	
	/* 创建网络传输
	 * 先关闭, 因为我们的开发板可能多次创建传输, 先关闭上次传输
	 */
	pNetDev->CloseTransfer(pNetDev);
	if (pNetDev->CreateTransfer(pNetDev, "udp", port) == 0)
	{
		printf("Create Transfer ok\r\n");
	}
	else
	{
		printf("Create Transfer err\r\n");
		return;
	}

	AddInputDevices();
	InitInputDevices();
	while (1)
	{
		if (GetInputEvent(&event) == 0)
		{
			if (event.eType == INPUT_EVENT_TYPE_KEY)
			{
				printf("get input event:\r\n");
				printf("type: %d\r\n", event.eType);
				printf("time: %d\r\n", event.time);
				printf("key : %d\r\n", event.iKey);
				printf("pressure : %d\r\n", event.iPressure);
			}
			else if (event.eType == INPUT_EVENT_TYPE_NET)
			{
				printf("get input event:\r\n");
				printf("type: %d\r\n", event.eType);
				printf("time: %d\r\n", event.time);
				printf("str : %s\r\n", event.str);
			}
		} 
	}

		
}



