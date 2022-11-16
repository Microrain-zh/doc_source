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
static struct rt_thread *thread3;   //定义线程3句柄指针

static struct rt_messagequeue mq;   //消息队列控制块
static rt_uint8_t msg_pool[2048];   //消息队列中用到的放置消息的内存池


/* 线程1的入口函数 */
static void thread1_entry(void *parameter)
{
	int result;
	int *buf = parameter;
	
	/* 线程1 */
	while(1)
	{
		/* 发送消息 */
		result = rt_mq_send(&mq, &buf, sizeof(int));
		if (result != RT_EOK)
		{
			rt_kprintf("rt_mq_send ERR\n");
		}
		rt_kprintf("rt_mq_send:%d\n\r", buf);	
		
		rt_thread_mdelay(10);
	}
}

/* 线程2的入口函数 */
static void thread2_entry(void *parameter)
{
	int buf = 0;
	
	/* 线程1 */
	while(1)
	{
        /* 接收消息 */
        if (rt_mq_recv(&mq, &buf, sizeof(buf), RT_WAITING_FOREVER) == RT_EOK)
        {
            rt_kprintf("rt_mq_recv:%d\n\r", buf);
        }
		rt_thread_mdelay(10);
	}
}


int main(void)
{
	rt_err_t result;

    /* 初始化消息队列 */
    result = rt_mq_init(&mq,                        //消息队列对象的句柄
                        "mqt",                      //消息队列的名字
                        &msg_pool[0],               //内存池指向msg_pool 
                        1,                         //每个消息的大小是1字节  
                        sizeof(msg_pool),           //内存池的大小是msg_pool的大小  
                        RT_IPC_FLAG_FIFO);          //如果有多个线程等待，按照先来先得到的方法分配消息 

    if (result != RT_EOK)
    {
        rt_kprintf("rt_mq_init ERR\n");
        return -1;
    }
	
	/* 创建动态线程thread1，优先级为 THREAD_PRIORIT = 15 */
    thread1 = rt_thread_create("thread1",          //线程名字
                            thread1_entry,         //入口函数
							(void *)100,           //入口函数参数
                            THREAD_STACK_SIZE,     //栈大小
                            THREAD_PRIORITY,       //线程优先级
				            THREAD_TIMESLICE);     //线程时间片大小

    /* 判断创建结果,再启动线程1 */
    if (thread1 != RT_NULL)
        rt_thread_startup(thread1);				   

	/* 创建动态线程thread2，优先级为 THREAD_PRIORIT = 15 */
    thread2 = rt_thread_create("thread2",          //线程名字
                            thread1_entry,         //入口函数
							(void *)200,           //入口函数参数
                            THREAD_STACK_SIZE,     //栈大小
                            THREAD_PRIORITY,       //线程优先级
				            THREAD_TIMESLICE);     //线程时间片大小

    /* 判断创建结果,再启动线程2 */
    if (thread2 != RT_NULL)
        rt_thread_startup(thread2);		
	
	/* 创建动态线程thread3，优先级为 THREAD_PRIORIT+1 = 16 */
    thread3 = rt_thread_create("thread3",          //线程名字
                            thread2_entry,         //入口函数
							NULL,                  //入口函数参数
                            THREAD_STACK_SIZE,     //栈大小
                            THREAD_PRIORITY + 1,   //线程优先级
				            THREAD_TIMESLICE);     //线程时间片大小

    /* 判断创建结果,再启动线程3 */
    if (thread3 != RT_NULL)
        rt_thread_startup(thread3);		
	
    return 0;
}
