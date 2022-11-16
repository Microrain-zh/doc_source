/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * Change Logs:
 * Date           Author         Notes
 * 2021-08-20     weidongshan    first version
 */

#include <rtthread.h>
#include <rtdevice.h>

#define THREAD_PRIORITY         15  //设置线程优先级
#define THREAD_STACK_SIZE       512 //设置线程栈大小
#define THREAD_TIMESLICE        15  //设置线程时间片大小

static struct rt_thread *thread1;   //定义线程1句柄指针
static struct rt_thread *thread2;   //定义线程2句柄指针

/* 线程1的入口函数 */
static void thread1_entry(void *parameter)
{
	const char *thread_name = parameter;
	volatile rt_uint32_t cnt = 0;
	rt_uint8_t priority;
	
	/* 线程 */
	while(1)
	{
		/* 打印线程的信息 */
		rt_kprintf("%s: thread1 priority is %d, thread2 priority is %d\n\r", 
						thread_name, thread1->current_priority, thread2->current_priority);
		
		if (thread1->current_priority <= thread2->current_priority)
		{
			priority = thread2->current_priority - 1;
			rt_thread_control(thread2, RT_THREAD_CTRL_CHANGE_PRIORITY, &priority);
			rt_schedule();
		}
		
	}
}

/* 线程2的入口函数 */
static void thread2_entry(void *parameter)
{
	const char *thread_name = parameter;
	volatile rt_uint32_t cnt = 0;
	
	/* 线程 */
	while(1)
	{
		/* 打印线程的信息 */
		rt_kprintf("%s: thread1 priority is %d, thread2 priority is %d\n\r", 
				thread_name, thread1->current_priority, thread2->current_priority);
	}
}

static const char *thread1_name = "Thread1 run";
static const char *thread2_name = "Thread2 run";

int main(void)
{
	/* 创建动态线程thread1，优先级为 THREAD_PRIORIT-1 = 14 */
    thread1 = rt_thread_create("thread1",          //线程名字
                            thread1_entry,         //入口函数
							(void *)thread1_name,  //入口函数参数
                            THREAD_STACK_SIZE,     //栈大小
                            THREAD_PRIORITY-1,     //线程优先级
				            THREAD_TIMESLICE);     //线程时间片大小

    /* 判断创建结果,再启动线程1 */
    if (thread1 != RT_NULL)
        rt_thread_startup(thread1);				   

	/* 创建动态线程thread2，优先级为 THREAD_PRIORIT = 15 */
    thread2 = rt_thread_create("thread2",          //线程名字
                            thread2_entry,         //入口函数
							(void *)thread2_name,  //入口函数参数
                            THREAD_STACK_SIZE,     //栈大小
                            THREAD_PRIORITY,       //线程优先级
				            THREAD_TIMESLICE);     //线程时间片大小

    /* 判断创建结果,再启动线程2 */
    if (thread2 != RT_NULL)
        rt_thread_startup(thread2);		
	
    return 0;
}
