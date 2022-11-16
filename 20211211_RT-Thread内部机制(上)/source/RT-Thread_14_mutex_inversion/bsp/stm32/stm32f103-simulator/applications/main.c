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

static struct rt_thread *LPThread;  // 低优先级线程句柄指针
static struct rt_thread *MPThread;   // 中优先级线程句柄指针
static struct rt_thread *HPThread;   // 高优先级线程句柄指针

static rt_mutex_t dynamic_mutex = RT_NULL; //定义互斥量句柄指针

static volatile rt_uint32_t LPThreadflag = 0;
static volatile rt_uint32_t MPThreadflag = 0;
static volatile rt_uint32_t HPThreadflag = 0;

static void set_thread_runflag(int which)
{
	if (which == 0)
	{
		LPThreadflag = 1;
		MPThreadflag = 0;		
		HPThreadflag = 0;		
	}
	else if (which == 1)
	{
		LPThreadflag = 0;
		MPThreadflag = 1;		
		HPThreadflag = 0;		
	}
	else if (which == 2)
	{
		LPThreadflag = 0;
		MPThreadflag = 0;		
		HPThreadflag = 1;		
	}
}

/* 低优先级线程的入口函数 */
static void lpthread_entry(void *parameter)
{
	const char *thread_name = parameter;
	
	while(1)
	{	
		set_thread_runflag(0);
		
		rt_mutex_take(dynamic_mutex, RT_WAITING_FOREVER);	
		
		set_thread_runflag(0);

		rt_kprintf("%s\n", thread_name);
		set_thread_runflag(0);
		
		rt_mutex_release(dynamic_mutex);		
	}
}

/* 中优先级线程的入口函数 */
static void mpthread_entry(void *parameter)
{
	const char *thread_name = parameter;
	
	set_thread_runflag(1);
	rt_thread_mdelay(1);
	
	while(1)
	{	
		set_thread_runflag(1);
		rt_kprintf("%s\n", thread_name);
	}
}

/* 高优先级线程的入口函数 */
static void hpthread_entry(void *parameter)
{
	const char *thread_name = parameter;
	
	set_thread_runflag(2);
	rt_thread_mdelay(3);
	
	while(1)
	{	
		set_thread_runflag(2);
		rt_mutex_take(dynamic_mutex, RT_WAITING_FOREVER);	
		
		set_thread_runflag(2);
		rt_kprintf("%s\n", thread_name);
		
		set_thread_runflag(2);		
		rt_mutex_release(dynamic_mutex);

		set_thread_runflag(2);
		rt_thread_mdelay(5);
	}
}

static const char *lpthread_name = "LPThread run";
static const char *mpthread_name = "MPThread run";
static const char *hpthread_name = "HPThread run";

int main(void)
{
	/* 创建一个动态互斥量 */
    dynamic_mutex = rt_mutex_create("dmutex", RT_IPC_FLAG_FIFO);
    if (dynamic_mutex == RT_NULL)
    {
        rt_kprintf("rt_mutex_create error.\n");
        return -1;
    }
	
	/* 创建动态线程LPThread，优先级为 THREAD_PRIORIT = 15 */
    LPThread = rt_thread_create("LPThread",         //线程名字
                            lpthread_entry,          //入口函数
							(void *)lpthread_name,  //入口函数参数
                            THREAD_STACK_SIZE,      //栈大小
                            THREAD_PRIORITY,        //线程优先级
				            THREAD_TIMESLICE);      //线程时间片大小

    /* 判断创建结果,再启动线程1 */
    if (LPThread != RT_NULL)
        rt_thread_startup(LPThread);	
	
	/* 创建动态线程MPThread，优先级为 THREAD_PRIORIT-1 = 14 */
    MPThread = rt_thread_create("MPThread",         //线程名字
                            mpthread_entry,         //入口函数
							(void *)mpthread_name,  //入口函数参数
                            THREAD_STACK_SIZE,     //栈大小
                            THREAD_PRIORITY-1,     //线程优先级
				            THREAD_TIMESLICE);     //线程时间片大小

    /* 判断创建结果,再启动线程2 */
    if (MPThread != RT_NULL)
        rt_thread_startup(MPThread);		
	
	/* 创建动态线程HPThread，优先级为 THREAD_PRIORIT-2 = 13 */
    HPThread = rt_thread_create("HPThread",         //线程名字
                            hpthread_entry,         //入口函数
							(void *)hpthread_name,  //入口函数参数
                            THREAD_STACK_SIZE,     //栈大小
                            THREAD_PRIORITY-2,     //线程优先级
				            THREAD_TIMESLICE);     //线程时间片大小
								
	/* 判断创建结果,再启动线程3 */
    if (HPThread != RT_NULL)
        rt_thread_startup(HPThread);	
	
    return 0;
}
