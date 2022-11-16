/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * Change Logs:
 * Date           Author         Notes
 * 2021-08-20     weidongshan    first version
 */
 
#include <string.h>
#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define THREAD_PRIORITY         15  //设置线程优先级
#define THREAD_STACK_SIZE       512 //设置线程栈大小
#define THREAD_TIMESLICE        15  //设置线程时间片大小

static struct rt_thread *thread1;   //定义线程1句柄指针
static struct rt_thread *thread2;   //定义线程2句柄指针

static struct rt_mailbox mb;        //邮箱控制块
static char mb_pool[128];           //放置邮件的内存池


/* 线程1的入口函数 */
static void thread1_entry(void *parameter)
{
	rt_err_t result;
	int count = 0;
	
	/* 线程1 */
	while(1)
	{
		/* 发送邮箱 */
		count++;
		result = rt_mb_send(&mb, (rt_ubase_t)count);
		if (result != RT_EOK)
		{
			rt_kprintf("rt_mb_send ERR\n");
		}
		rt_kprintf("rt_mb_send:%d\n\r", count);	
		
		rt_thread_mdelay(10);
	}
}

/* 线程2的入口函数 */
static void thread2_entry(void *parameter)
{
	int val;
	
	/* 线程2 */
	while(1)
	{
        /* 接收消息 */
        if (rt_mb_recv(&mb, (rt_ubase_t *)&val, RT_WAITING_FOREVER) == RT_EOK)
        {
            rt_kprintf("rt_mb_recv:%d\n\r", val);
        }
		rt_thread_mdelay(5);
	}
}

int main(void)
{
	rt_err_t result;

    /* 初始化邮箱 */
    result = rt_mb_init(&mb,                       //邮箱对象的句柄
                        "mbt",                     //邮箱的名字
                        &mb_pool[0],               //内存池指向mb_pool
                        sizeof(mb_pool) / 4,       //邮箱中能容纳的邮件数量,每封邮件占四字节  
                        RT_IPC_FLAG_FIFO);         //如果有多个线程等待，按照先来先得到的方法分配邮件 

    if (result != RT_EOK)
    {
        rt_kprintf("rt_mb_init ERR\n");
        return -1;
    }
	
	/* 创建动态线程thread1，优先级为 THREAD_PRIORIT-1 = 14 */
    thread1 = rt_thread_create("thread1",          //线程名字
                            thread1_entry,         //入口函数
							RT_NULL,               //入口函数参数
                            THREAD_STACK_SIZE,     //栈大小
                            THREAD_PRIORITY - 1,   //线程优先级
				            THREAD_TIMESLICE);     //线程时间片大小

    /* 判断创建结果,再启动线程1 */
    if (thread1 != RT_NULL)
        rt_thread_startup(thread1);				   

	/* 创建动态线程thread2，优先级为 THREAD_PRIORIT = 15 */
    thread2 = rt_thread_create("thread2",          //线程名字
                            thread2_entry,         //入口函数
							RT_NULL,               //入口函数参数
                            THREAD_STACK_SIZE,     //栈大小
                            THREAD_PRIORITY,       //线程优先级
				            THREAD_TIMESLICE);     //线程时间片大小

    /* 判断创建结果,再启动线程2 */
    if (thread2 != RT_NULL)
        rt_thread_startup(thread2);		
			
    return 0;
}
