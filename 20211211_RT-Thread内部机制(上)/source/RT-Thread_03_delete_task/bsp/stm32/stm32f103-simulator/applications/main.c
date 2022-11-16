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

static char thread1_stack[1024];    //设置线程1的内存栈
static struct rt_thread thread1;    //定义线程1句柄

static struct rt_thread *thread2;   //定义线程2句柄指针


/* 线程的入口函数 */
static void thread1_entry(void *parameter)
{
	const char *thread_name = parameter;
	volatile rt_uint32_t cnt = 0;
	
	/* 线程 */
	while(1)
	{
		/* 打印线程的信息 */
		rt_kprintf(thread_name);
		
		/* 延迟一会(比较简单粗暴) */
		for( cnt = 0; cnt < 100000; cnt++ )
		{
		}
	}
}

static const char *thread1_name = "Thread1 run\r\n";
static const char *thread2_name = "Thread2 run\r\n";

int main(void)
{
	rt_err_t result;
	volatile rt_uint32_t cnt = 0;
	
	/* 初始化静态线程1，名称是Thread1，入口是thread1_entry */
    rt_thread_init(&thread1,               //线程句柄 
                   "thread1",              //线程名字
                   thread1_entry,          //入口函数
                   (void *)thread1_name,   //入口函数参数
                   &thread1_stack[0],      //线程栈起始地址
                   sizeof(thread1_stack),  //栈大小
                   THREAD_PRIORITY,        //线程优先级
				   THREAD_TIMESLICE);      //线程时间片大小     
	/* 启动线程1 */
    rt_thread_startup(&thread1);           
	
	
	/* 创建动态线程2，名称是thread2，入口也是thread1_entry*/
    thread2 = rt_thread_create("thread2",          //线程名字
                            thread1_entry,         //入口函数
							(void *)thread2_name,  //入口函数参数
                            THREAD_STACK_SIZE,     //栈大小
                            THREAD_PRIORITY,       //线程优先级
				            THREAD_TIMESLICE);     //线程时间片大小

    /* 判断创建结果,再启动线程2 */
    if (thread2 != RT_NULL)
        rt_thread_startup(thread2);				   

	/* 延时一段时间后再删除线程 */
	rt_thread_mdelay(100);
	
	/* 删除线程1 */
	result =  rt_thread_detach(&thread1); 
	if(result == RT_EOK)
		rt_kprintf("Thread1 exit\r\n");
	
	/* 删除线程2 */
	result =  rt_thread_delete(thread2); 
	if(result == RT_EOK)
		rt_kprintf("Thread2 exit\r\n");
	
    return 0;
}
