/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * Change Logs:
 * Date           Author         Notes
 * 2021-08-20     weidongshan    first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define THREAD_PRIORITY         15  //设置线程优先级
#define THREAD_STACK_SIZE       512 //设置线程栈大小
#define THREAD_TIMESLICE        15  //设置线程时间片大小

static struct rt_thread *cooking_thread;   //定义线程1句柄指针
static struct rt_thread *firing_thread;   //定义线程2句柄指针
static struct rt_thread *washing_thread;   //定义线程2句柄指针


#define EVENT_WASH_FLAG10 (1 << 10)
#define EVENT_FIRE_FLAG15 (1 << 15)
static struct rt_event event;       //事件控制块

/* Cooking线程的入口函数 */
static void cooking_thread_entry(void *parameter)
{
	rt_uint32_t e;
	
	while(1)
	{		
		//接收到washing和firing后才能cooking
		if (rt_event_recv(&event, (EVENT_WASH_FLAG10 | EVENT_FIRE_FLAG15),  //触发标志
						  RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,          //需要两个同时满足
						  RT_WAITING_FOREVER, &e) == RT_EOK)                //永不超时
		{
			rt_kprintf("Cooking OK!\n\r");
		}
	}

}

/* Firing线程的入口函数 */
static void firing_thread_entry(void *parameter)
{
	rt_uint32_t e;
	
	//接收到washing后才能firing
	while(1)
	{		
		if (rt_event_recv(&event, EVENT_WASH_FLAG10,                   //触发标志
						  RT_EVENT_FLAG_AND,                           //任意方式
						  RT_WAITING_FOREVER, &e) == RT_EOK)           //永不超时
		{
			rt_kprintf("Firing OK!\n\r");
			rt_event_send(&event, EVENT_FIRE_FLAG15);                  //发送事件Firing
		}
	}
}

/* Washing线程的入口函数 */
static void washing_thread_entry(void *parameter)
{
	while(1)
	{
		rt_kprintf("Washing OK!\n\r");
		rt_event_send(&event, EVENT_WASH_FLAG10); //发送事件Washing

		rt_thread_mdelay(500);	
	}
}

static const char *cooking_thread_name = "Cooking Thread run\n\r";
static const char *firing_thread_name = "Firing Thread run\n\r";
static const char *washing_thread_name = "Washing Thread run\n\r";

int main(void)
{
	/* 创建一个静态事件集 */
	if (rt_event_init(&event, "event", RT_IPC_FLAG_FIFO)!= RT_EOK)
    {
        rt_kprintf("rt_mutex_create error.\n");
        return -1;
    }
	
	/* 创建动态线程cooking thread，优先级为 THREAD_PRIORIT-2 = 13 */
    cooking_thread = rt_thread_create("CookingThread",    //线程名字
                            cooking_thread_entry,         //入口函数
							(void *)cooking_thread_name,  //入口函数参数
                            THREAD_STACK_SIZE,            //栈大小
                            THREAD_PRIORITY-2,            //线程优先级
				            THREAD_TIMESLICE);            //线程时间片大小

    /* 判断创建结果,再启动cooking线程 */
    if (cooking_thread != RT_NULL)
        rt_thread_startup(cooking_thread);	
	
	/* 创建动态线程firing thread，优先级为 THREAD_PRIORIT-1 = 14 */
    firing_thread = rt_thread_create("FiringThread",     //线程名字
                            firing_thread_entry,         //入口函数
							(void *)firing_thread_name,  //入口函数参数
                            THREAD_STACK_SIZE,           //栈大小
                            THREAD_PRIORITY-1,           //线程优先级
				            THREAD_TIMESLICE);           //线程时间片大小

    /* 判断创建结果,再启动firing线程 */
    if (firing_thread != RT_NULL)
        rt_thread_startup(firing_thread);		
	
	/* 创建动态线程washing thread，优先级为 THREAD_PRIORIT = 15 */
    washing_thread = rt_thread_create("WashingThread",    //线程名字
                            washing_thread_entry,         //入口函数
							(void *)washing_thread_name,  //入口函数参数
                            THREAD_STACK_SIZE,            //栈大小
                            THREAD_PRIORITY,              //线程优先级
				            THREAD_TIMESLICE);            //线程时间片大小
								
	/* 判断创建结果,再启动washing线程 */
    if (washing_thread != RT_NULL)
        rt_thread_startup(washing_thread);	
	
    return 0;
}
