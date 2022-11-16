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

static struct rt_thread *thread1;   //定义线程1句柄指针
static struct rt_thread *thread2;   //定义线程2句柄指针

static rt_sem_t dynamic_sem = RT_NULL; //定义信号量句柄指针


/* 线程1的入口函数 */
static void thread1_entry(void *parameter)
{
	const char *thread_name = parameter;
	
	/* 打印线程的信息 */
	rt_kprintf(thread_name);
	
	while(1)
	{	
		if(rt_sem_release(dynamic_sem) == RT_EOK) //释放信号量
		{
			rt_kprintf("rt_sem_release: value=%d\n", dynamic_sem->value);		
		}
		rt_thread_mdelay(500);
	}
}

/* 线程2的入口函数 */
static void thread2_entry(void *parameter)
{
	const char *thread_name = parameter;
	
	/* 打印线程的信息 */
	rt_kprintf(thread_name);
	
	while(1)
	{		
		if(rt_sem_take(dynamic_sem, RT_WAITING_FOREVER) == RT_EOK) //获取信号量(不超时退出)
		{
			rt_kprintf("rt_sem_take: value=%d\n", dynamic_sem->value);
		}
		else
		{
			rt_kprintf("rt_sem_take: errot\n");
			rt_sem_delete(dynamic_sem);
			return;
		}
		rt_thread_mdelay(100);
	}
}

static const char *thread1_name = "Thread1 run\r\n";
static const char *thread2_name = "Thread2 run\r\n";

int main(void)
{
	/* 创建一个动态信号量，初始值是0 */
    dynamic_sem = rt_sem_create("dsem", 0, RT_IPC_FLAG_FIFO);
    if(dynamic_sem == RT_NULL)
    {
        rt_kprintf("rt_sem_create error.\n");
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
