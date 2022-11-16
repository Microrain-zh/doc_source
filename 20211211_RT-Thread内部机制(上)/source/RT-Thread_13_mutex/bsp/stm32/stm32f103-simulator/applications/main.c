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

#define USE_MUTEX 1                 //互斥量开关

#define THREAD_PRIORITY         15  //设置线程优先级
#define THREAD_STACK_SIZE       512 //设置线程栈大小
#define THREAD_TIMESLICE        15  //设置线程时间片大小

static struct rt_thread *thread1;   //定义线程1句柄指针
static struct rt_thread *thread2;   //定义线程2句柄指针

static rt_mutex_t dynamic_mutex = RT_NULL; //定义互斥量句柄指针


/* 线程1的入口函数 */
static void thread1_entry(void *parameter)
{
	static rt_uint32_t i = 0;
	const char *thread_name = parameter;
	
	/* 打印线程的信息 */
	rt_kprintf(thread_name);
	
	while(1)
	{	
		
#ifdef USE_MUTEX
	rt_mutex_take(dynamic_mutex, RT_WAITING_FOREVER);	
	
	for(i=0;i<20;i++)
		rt_kprintf("thread1 USE_MUTEX: i=%d\n", i);	
	
	rt_mutex_release(dynamic_mutex);	
#else
	for(i=0;i<20;i++)
		rt_kprintf("thread1 NO_USE_MUTEX: i=%d\n", i);			
#endif
	}
}

/* 线程2的入口函数 */
static void thread2_entry(void *parameter)
{
	static rt_uint32_t i = 0;
	const char *thread_name = parameter;
	
	/* 打印线程的信息 */
	rt_kprintf(thread_name);
	
	while(1)
	{	
		
#ifdef USE_MUTEX
	rt_mutex_take(dynamic_mutex, RT_WAITING_FOREVER);	
	
	for(i=0;i<20;i++)
		rt_kprintf("thread2 USE_MUTEX: i=%d\n", i);	
	
	rt_mutex_release(dynamic_mutex);	
#else
	for(i=0;i<20;i++)
		rt_kprintf("thread2 NO_USE_MUTEX: i=%d\n", i);			
#endif
	}
}

static const char *thread1_name = "Thread1 run\r\n";
static const char *thread2_name = "Thread2 run\r\n";

int main(void)
{
	/* 创建一个动态互斥量 */
    dynamic_mutex = rt_mutex_create("dmutex", RT_IPC_FLAG_FIFO);
    if (dynamic_mutex == RT_NULL)
    {
        rt_kprintf("rt_mutex_create error.\n");
        return -1;
    }
	
	/* 创建动态线程thread1，优先级为 THREAD_PRIORIT = 15 */
    thread1 = rt_thread_create("thread1",          //线程名字
                            thread1_entry,         //入口函数
							(void *)thread1_name,  //入口函数参数
                            THREAD_STACK_SIZE,     //栈大小
                            THREAD_PRIORITY,       //线程优先级
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
