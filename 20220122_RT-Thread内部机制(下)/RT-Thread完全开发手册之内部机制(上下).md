# RT-Thread完全开发手册之内部机制

## 资料下载

![image-20211211082918357](pic/pre/04_download.png)

## 课程介绍

对于 RT-Thread 的掌握可以分为 3 个层次：

* 第 1 层：知道怎么使用相关 API 函数
* 第 2 层：知道内部机制
* 第 3 层：掌握代码实现的细节，能够移植

百问网科技在嵌入式操作系统领域深耕 13 年，能够使用 2 天(每天 6 小时)让你达到第 2 层。
这让你能够在日常开发中使用 RT-Thread 并解决疑难问题。
12 月 18 号 RT-Thread 开发者大会即将在深圳举行，作为 RT-Thread 的合作伙伴，我们将在12月11号全天举行一场技术直播，讲解 RT-Thread 的内部机制。

RT-Thread 的线程管理与调度、线程间通信(邮箱/消息队列/信号)、线程间同步(信号量/互  斥量/事件集)的方法，核心都是：

* 链表
* 定时器

基于链表和定时器，可以快速而深入地理解RT-Thread 。



## 1. RTOS 概念及线程的引入

### 1.1 RTOS的概念

#### 1.1.1 用人来类比单片机程序和RTOS

![](pic/pre/01_mother_do_jobs.png)

妈妈要一边给小孩喂饭，一边加班跟同事微信交流，怎么办？

对于单线条的人，不能分心、不能同时做事，她只能这样做：

* 给小孩喂一口饭
* 瞄一眼电脑，有信息就去回复
* 再回来给小孩喂一口饭
* 如果小孩吃这口饭太慢，她回复同事的信息也就慢了，被同事催：你半天都不回我？
* 如果回复同事的信息要写一大堆，小孩就着急得大哭起来。

这种做法，在软件开发上就是一般的单片机开发，没有用操作系统。



对于眼明手快的人，她可以一心多用，她这样做：

* 左手拿勺子，给小孩喂饭
* 右手敲键盘，回复同事
* 两不耽误，小孩“以为”妈妈在专心喂饭，同事“以为”她在专心聊天
* 但是脑子只有一个啊，虽然说“一心多用”，但是谁能同时思考两件事？
* 只是她反应快，上一秒钟在考虑夹哪个菜给小孩，下一秒钟考虑给同事回复什么信息

 

这种做法，在软件开发上就是使用操作系统，在单片机里叫做使用RTOS。

RTOS的意思是：Real-time operating system，实时操作系统。

我们使用的Windows也是操作系统，被称为通用操作系统。使用Windows时，我们经常碰到程序卡死、停顿的现象，日常生活中这可以忍受。

但是在电梯系统中，你按住开门键时如果没有即刻反应，即使只是慢个1秒，也会夹住人。

在专用的电子设备中，“实时性”很重要。



#### 1.1.2 程序简单示例

```c
// 经典单片机程序
void main()
{
	while (1)
    {
        喂一口饭();
        回一个信息();
    }
}
------------------------------------------------------
// RTOS程序    
int a;    
喂饭() 栈A
{
    int b = 2;
    int c;
    c = b+2;==> 1. b+2,  2. c = new val
    ---------------> 切换
    while (1)
    {
        喂一口饭();
    }
}

回信息()  栈B
{
    int b;
    while (1)
    {
        回一个信息();
    }
}

void main()
{
    create_task(喂饭);
    create_task(回信息);
    start_scheduler();
    while (1)
    {
        sleep();
    }
}
```



![image-20210712065106008](pic/pre/02_program_with_rtos.png)



#### 1.1.3 提出问题

什么叫线程？回答这个问题之前，先想想怎么切换线程？怎么保存线程？

* 线程是函数吗？函数需要保存吗？函数在Flash上，不会被破坏，无需保存
* 函数执行到了哪里？需要保存吗？需要保存
* 函数里用到了全局变量，全局变量需要保存吗？全局变量在内存上，还能保存到哪里去？全局变量无需保存
* 函数里用到了局部变量，局部变量需要保存吗？局部变量在栈里，也是在内存里，只要避免栈不被破坏即可，局部变量无需保存
* 运算的中间值需要保存吗？中间值保存在哪里？在CPU寄存器里，另一个线程也要用到CPU寄存器，所以CPU寄存器需要保存
* 函数运行了哪里：它也是一个CPU寄存器，名为"PC"
* 汇总：CPU寄存器需要保存！
* 保存在哪里？保存在线程的栈里
* 怎么理解CPU寄存器、怎么理解栈？



### 1.2 ARM 架构及汇编

#### 1.2.1 ARM架构

ARM芯片属于精简指令集计算机(RISC：Reduced Instruction Set Computor)，它所用的指令比较简单，有如下特点：

① 对内存只有读、写指令

② 对于数据的运算是在CPU内部实现

③ 使用RISC指令的CPU复杂度小一点，易于设计

比如对于a=a+b这样的算式，需要经过下面4个步骤才可以实现：

![image-20211211070259111](pic/arm/01_cpu_ram.png)

细看这几个步骤，有些疑问：

① 读a，那么a的值读出来后保存在CPU里面哪里？

② 读b，那么b的值读出来后保存在CPU里面哪里？

③ a+b的结果又保存在哪里？

我们需要深入ARM处理器的内部。简单概括如下，我们先忽略各种CPU模式(系统模式、用户模式等等)。

![image-20211211070449003](pic/arm/02_cpu_registers.png)

CPU运行时，先去取得指令，再执行指令：

① 把内存a的值读入CPU寄存器R0

② 把内存b的值读入CPU寄存器R1

③ 把R0、R1累加，存入R0

④ 把R0的值写入内存a



CPU内部有r0到r15寄存器，这些寄存器有别名(下图来自[百度文库](https://wenku.baidu.com/view/2e9d9c7e941ea76e58fa04ef.html))：

![image-20211211072941967](pic/arm/03_cpu_reg_names.png)



#### 1.2.2 几条汇编指令

需要我们掌握的汇编指令并不多，只有几条：

* 读内存指令：LDR，即Load之意
* 写内存指令：STR，即Store之意
* 加减指令：ADD、SUB
* 跳转：BL，即Branch And Link
* 入栈指令：PUSH
* 出栈指令：POP



视频演示。

汇编并不复杂：

> **加载/存储指令(LDR/STR)**
>
> - 加载指令LDR： `LDR r0,[addrA]`  意思是将地址addrA的内容加载(存放)到r0里面
> - 存储指令STR： `STR r0,[addrA]`  意思是将r0中的值存储到地址addrA上
>
> **加法运算指令(ADD)**
>
> - 加法运算指令(ADD)： `ADD r0,r1,r2`  意思为：r0=r1+r2
> - 减法运算指令(SUB)： `SUB r0,r1,r2`  意思为：r0=r1-r2
>
> **寄存器入栈/出栈指令(PUSH/POP)**
>
> - 寄存器入栈(PUSH)： `PUSH {r3，lr}`  意思是将寄存器r3和pc写入内存栈
>   * 本质是写内存STR指令，高标号寄存器写入高地址的栈里，低标号寄存器写入低地址的栈里
>   * lr即r14，写入地址为`sp-4`的内存，然后：sp=sp-4
>   * r3，写入地址为`sp-4`的内存，然后：sp=sp-4
> - 寄存器出栈指令(POP)： `POP {r3，pc}`  意思是取出内存栈的数据放入r3和pc中
>   * 本质是读内存LDR指令，高标号寄存器的内容来自高地址的栈，低标号寄存器的内容来自低地址的栈
>   * 读地址为`sp`的内存存入r3，然后：sp=sp+4
>   * 读地址为`sp`的内存存入pc，然后：sp=sp+4
>



### 1.3 函数运行的本质

如下是一个简单的程序，在配套源码的`01_arm_stack`。

主函数里调用函数`add_val()`：

```c
void add_val(int *pa, int *pb)
{
	volatile int tmp;
	
	tmp = *pa;
	tmp = tmp + *pb;
	*pa = tmp;
}


int main()
{
	int a = 1;
	int b = 2;
	
	add_val(&a, &b);

	return 0;
}
```



其中调用函数`add_val()`的汇编代码如下，我们加上了注释：

```c
1   /* enter */ 
2   PUSH     {r3,lr}         //进入函数，寄存器r3、lr的值，都存入内存的栈中(lr保存程序返回地址)
3  
4   /* tmp = *pa; */ 
5   LDR      r2,[r0,#0x00]   //将寄存器r0的值存放到r2，其中r0是函数的第一个参数值(ARM ATPCS规定)
6   STR      r2,[sp,#0x00]   //将寄存器r2的值存储到sp指向的内存
7 
8   /* tmp = tmp + *pb;  */
9   LDR      r2,[r1,#0x00]   //将寄存器r1的值存放到r2，其中r1是函数的第二个参数值(ARM ATPCS规定)
10  LDR      r3,[sp,#0x00]   //将寄存器sp指向内存的值存放到r3
11  ADD      r2,r2,r3        //将寄存器r2和r3相加，保存到r2
12  STR      r2,[sp,#0x00]   //将寄存器r2的值存储到sp指向的内存 
13 
14  /* *pa = tmp; */ 
15  LDR      r2,[sp,#0x00]   //将寄存器sp指向内存的值存放到r2
16  STR      r2,[r0,#0x00]   //将寄存器r2的值存储到r0，其中r0将作为函数返回值(ARM ATPCS规定)
17  
18  /* quit */
19  POP      {r3,pc}         //退出函数，获取内存的栈中的数据放入r3和pc中(此时pc为lr,实现了函数返回)
```



视频里动态演示，重点演示栈的使用。



### 1.4 什么叫线程？ 怎么保存线程？

现在可以回答这个问题了。

什么叫线程：运行中的函数、被暂停运行的函数

怎么保存线程：把暂停瞬间的CPU寄存器值，保存进栈里。



视频里动态演示：在函数的某个位置，怎么保存当前环境？





## 2. 创建线程的函数

![image-20211211075232633](pic/task/01_task_create_example.png)

### 2.1 参数解析

* 线程做什么？需要提供函数
* 线程随时会别切换，哪些寄存器保存在哪里？需要提供栈：可以实现分配(比如使用数组)，也可以动态分配
* 怎么记录这些信息：栈在哪里？需要有一个线程结构体



#### 2.1.1 线程结构体

rt_thread用来表示一个线程，它的重要成员如下：

* thread->entry：函数指针
* thread->parameter：函数参数
* thread->stack_addr：栈的起始地址
* thread->stack_size：栈大小
* thread->sp：栈顶
* thread->init_priority：初始优先级
* thread->current_priority：当前优先级
* thread->init_tick：一次能运行多少个tick
* thread->remaining_tick：当次运行还剩多少个tick

```c

/**
 * Thread structure
 */
struct rt_thread
{
    /* rt object */
    char        name[RT_NAME_MAX];                      /**< the name of thread */
    rt_uint8_t  type;                                   /**< type of object */
    rt_uint8_t  flags;                                  /**< thread's flags */

#ifdef RT_USING_MODULE
    void       *module_id;                              /**< id of application module */
#endif

    rt_list_t   list;                                   /**< the object list */
    rt_list_t   tlist;                                  /**< the thread list */

    /* stack point and entry */
    void       *sp;                                     /**< stack point */
    void       *entry;                                  /**< entry */
    void       *parameter;                              /**< parameter */
    void       *stack_addr;                             /**< stack address */
    rt_uint32_t stack_size;                             /**< stack size */

    /* error code */
    rt_err_t    error;                                  /**< error code */

    rt_uint8_t  stat;                                   /**< thread status */

    /* priority */
    rt_uint8_t  current_priority;                       /**< current priority */
    rt_uint8_t  init_priority;                          /**< initialized priority */
#if RT_THREAD_PRIORITY_MAX > 32
    rt_uint8_t  number;
    rt_uint8_t  high_mask;
#endif
    rt_uint32_t number_mask;

#if defined(RT_USING_EVENT)
    /* thread event */
    rt_uint32_t event_set;
    rt_uint8_t  event_info;
#endif

#if defined(RT_USING_SIGNALS)
    rt_sigset_t     sig_pending;                        /**< the pending signals */
    rt_sigset_t     sig_mask;                           /**< the mask bits of signal */

#ifndef RT_USING_SMP
    void            *sig_ret;                           /**< the return stack pointer from signal */
#endif
    rt_sighandler_t *sig_vectors;                       /**< vectors of signal handler */
    void            *si_list;                           /**< the signal infor list */
#endif

    rt_ubase_t  init_tick;                              /**< thread's initialized tick */
    rt_ubase_t  remaining_tick;                         /**< remaining tick */

    struct rt_timer thread_timer;                       /**< built-in thread timer */

    void (*cleanup)(struct rt_thread *tid);             /**< cleanup function when thread exit */

    /* light weight process if present */
#ifdef RT_USING_LWP
    void        *lwp;
#endif

    rt_uint32_t user_data;                             /**< private user data beyond this thread */
};
```



### 2.2 创建线程的过程

创建线程的过程，就是构造栈的过程。

函数调用关系如下：

```shell
rt_thread_create
		// 1. 分配线程结构体
	    thread = (struct rt_thread *)rt_object_allocate(RT_Object_Class_Thread,
                                                    name);
                                                    
		// 2. 分配栈
		stack_start = (void *)RT_KERNEL_MALLOC(stack_size);
		
		// 3. 初始化栈，即构造栈的内容
		_rt_thread_init
			// 3.1 具体操作
			thread->sp = (void *)rt_hw_stack_init

```

视频里分析这个函数：

![image-20211211080503438](pic/task/02_stack_init.png)



## 3. 线程的调度机制(核心是链表与定时器)

### 3.1 使用链表来管理线程

有很多线程都想运行，优先级各不相同，怎么管理它们？

```c
rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];
```

每个优先级，都有一个就绪链表：rt_thread_priority_table[优先级]，

线程被创建后，要使用`rt_thread_startup()`来启动它，调用过程为：

```c
rt_thread_startup
    rt_thread_resume(thread);
		rt_schedule_insert_thread(thread);
            /* insert thread to ready list */
            rt_list_insert_before(&(rt_thread_priority_table[thread->current_priority]),
                                  &(thread->tlist));			
```



### 3.2 使用链表来理解调度机制

#### 3.2.1 第1个线程是谁？

最高优先级的、ready list里的、第1个线程：

![image-20211211081634409](pic/task/03_scheduler_start.png)



#### 3.2.2 抢占

最高优先级的、ready list里的、第1个线程：

![image-20211211081823476](pic/task/04_rt_schedule.png)





### 3.3 使用链表和Tick来理解时间片轮转

FreeRTOS的任务轮转时：每个任务运行一个Tick。

RT-Thread的线程轮转时：每个线程可以运行指定的Tick(创建线程时指定了时间片)：

![image-20211211124800284](pic/task/07_init_tick.png)

在线程结构体中会记录tick数值：它运行多少个Tick，才会让出CPU资源。

创建线程时，会调用到函数`_rt_thread_init()`，里面会设置Tick参数：

```c
    /* tick init */
    thread->init_tick      = tick;
    thread->remaining_tick = tick;
```

当前任务还可以运行多少个tick？在thread->remaining_tick中记录；

每发生一次tick中断，thread->remaining_tick减1；

当thread->remaining_tick等于0时，要让出CPU：调用`rt_thread_yield()`

![image-20211211082027452](pic/task/05_tick_inc.png)

`rt_thread_yield()`就是把当前、用完时间的线程，放到就绪链表的最后：

![image-20211211082346197](pic/task/06_yield.png)

注意：

* 如果有更高优先级线程，即使当前线程是时间片没用完，也会立刻被抢占
* 当它被抢占后，再次运行时：继续运行完剩下的时间




### 3.5 线程状态的切换(链表+定时器)

参考程序：`RT-Thread_06_taskdelay`

#### 3.5.1 线程状态切换图

![image-20211211130716642](pic/task/08_thread_status.png)

#### 3.5.2 核心：链表和定时器

每个线程内部都有一个定时器，它的作用非常大：所有的超时函数都依赖它。

```c
struct rt_thread
{
    ......
    struct rt_timer thread_timer;                       /**< built-in thread timer */
    ......
}
```



以`rt_thread_mdelay(50)`为例，它会让当前线程挂起50ms，会先把50ms转换为Tick个数。

也可以使用`rt_thread_delay(50)`，它会让当前线程挂起50个Tick。

挂起过程：

* 把当前线程从就绪链表中取出来
* 设定定时器：
  * 在创建线程时已经设置了超时函数
    ![image-20211211133148800](pic/task/10_thread_timer_init.png)
  * thread_timer->timeout_tick = rt_tick_get() + timer->init_tick;  // 当前Tick加上某个数值
  * 把定时器放入链表: rt_timer_list[xxx]
* 切换线程：rt_schedule

被唤醒过程：

* 系统每隔一定时间产生Tick中断，Tick中断函数会调用`rt_tick_increase()`
  ![image-20211211132949941](pic/task/09_check_timer.png)

* rt_timer_check功能

  * 检查rt_timer_list[xxx]链表，找出超时的定时器，把它从链表中移除

  * 调用定时器的超时函数：`t->timeout_func(t->parameter);`，对于线程自带的定时器，这个函数就是`rt_thread_timeout`

    ![image-20211211133724706](pic/task/11_rt_thread_timeout.png)





## 4. 消息队列(queue)

![](pic/queue/03_queue_detail.png)



### 4.1 核心是：链表和定时器

有两个链表：

* 写队列不成功而挂起
* 读队列不成功而挂起

![image-20211211134456898](pic/queue/01_queue_list.png)



### 4.2 操作示例

参考程序：`RT-Thread_08_queue`

#### 4.2.1 写队列

调用过程：

```c
rt_mq_send_wait
        /* 如果不成功：当前线程挂起, 并放入链表: mq->suspend_sender_thread */
        rt_ipc_list_suspend(&(mq->suspend_sender_thread),
                            thread,
                            mq->parent.parent.flag);

		/* 如果指定了超时时间: 最多等多久 
		 * 设置、启动线程定时器
		 */
		rt_timer_start(&(thread->thread_timer));
    
```



一个线程写队列时，如果队列已经满了，它会被挂起，何时被唤醒？

* 超时：注意thread->errro等于"-RT_ETIMEOUT"
  ![](pic/task/11_rt_thread_timeout.png)
* 别的线程读队列: 注意thread->errro等于默认值
  ![image-20211211135341056](pic/queue/02_mq_recv.png)





## 5. 邮箱(mailbox)

示例程序：`RT-Thread_10_queue_mailbox`

消息队列和邮箱稍有差别：

* 消息队列：使用memcpy传递数据，数据大小在初始化队列时确定(可以是任意大小)
* 邮箱：只写读值、写值，数据大小是rt_ubase_t(一般是unsigned long)

如果我们只是传递很小的数据，比如一些数值，可以使用邮箱：它的效率更高。

### 5.1 核心是：链表、定时器、环形buff

有两个链表：

* 写邮箱不成功而挂起
* 读邮箱不成功而挂起

![image-20220122074711008](pic/mailbox/01_struct.png)

定时器：

* 在线程结构体里面
* 用来实现超时机制



### 5.2 操作示例

示例程序：`RT-Thread_10_queue_mailbox`

#### 5.2.1 创建邮箱

![image-20220122075033725](pic/mailbox/02_mb_create.png)

#### 5.2.2 写邮箱

调用过程：

```c
rt_mb_send_wait
    /* disable interrupt */
    /* 简单粗暴: 关中断 */
    temp = rt_hw_interrupt_disable();

    /* mailbox is full */
	/* 邮箱满? */
    while (mb->entry == mb->size)
    {
        /* 省略 */
        
        /* suspend current thread */
        /* 当前线程: 1.休眠, 2.把自己记录在邮箱的suspend_sender_thread链表里 */
        rt_ipc_list_suspend(&(mb->suspend_sender_thread),
                            thread,
                            mb->parent.parent.flag);        
    }
        
	/* 写数据 */
    /* set ptr */
    mb->msg_pool[mb->in_offset] = value;
    /* increase input offset */
    ++ mb->in_offset;
    if (mb->in_offset >= mb->size)
        mb->in_offset = 0;

	/* 有线程在等待邮箱的数据吗? 有的话唤醒它 */
    /* resume suspended thread */
    if (!rt_list_isempty(&mb->parent.suspend_thread))
    {
        rt_ipc_list_resume(&(mb->parent.suspend_thread));

        /* enable interrupt */
        rt_hw_interrupt_enable(temp);

        rt_schedule();

        return RT_EOK;
    }


```

#### 5.2.3 读邮箱

调用过程：

```c
rt_mb_recv
    /* 简单粗暴: 关中断 */
    /* disable interrupt */
    temp = rt_hw_interrupt_disable();

	/* 邮箱空? */
    /* mailbox is empty */
    while (mb->entry == 0)
    {
        /* 省略其他代码 */
        
        /* 当前线程: 1.休眠, 2.把自己记录在邮箱的parent.suspend_thread链表里 */
        /* suspend current thread */
        rt_ipc_list_suspend(&(mb->parent.suspend_thread),
                            thread,
                            mb->parent.parent.flag);

    }

	/* 读数据 */
    /* fill ptr */
    *value = mb->msg_pool[mb->out_offset];

    /* increase output offset */
    ++ mb->out_offset;
    if (mb->out_offset >= mb->size)
        mb->out_offset = 0;

	/* 有线程在等待空间写邮箱? 唤醒它！ */
    /* resume suspended thread */
    if (!rt_list_isempty(&(mb->suspend_sender_thread)))
    {
        rt_ipc_list_resume(&(mb->suspend_sender_thread));

```





## 6. 信号量(semaphore)

队列、邮箱用来传递数据。

如果只是用来传递资源的个数，可以使用信号量。

比如停车场，下图的场景用信号量效率更高：

![image-20220122080457265](pic/semaphore/01_usage.png)



### 6.1 核心是：链表和定时器

只有1个链表：

* 等待资源不成功而挂起
* 释放资源：无需挂起

![image-20220122081027164](pic/semaphore/02_struct.png)

定时器：

* 在线程结构体里面
* 用来实现超时机制

### 6.2 操作示例

示例程序：`RT-Thread_12_semaphore`

#### 6.2.1 创建信号量

![image-20220122081219742](pic/semaphore/03_semaphore_create.png)



#### 6.2.2 释放信号量

调用过程如下：

```c
rt_sem_release
    /* 简单粗暴: 关中断 */
    /* disable interrupt */
    temp = rt_hw_interrupt_disable();

	/* 有线程在等待信号量? */
    if (!rt_list_isempty(&sem->parent.suspend_thread))
    {
        /* 直接唤醒它, 也不用去修改资源个数了 */
        /* resume the suspended thread */
        rt_ipc_list_resume(&(sem->parent.suspend_thread));
        need_schedule = RT_TRUE;
    }

    else /* 没有线程在等待信号量? */
    {
        /* 累加资源个数 */
        if(sem->value < RT_SEM_VALUE_MAX)
        {
            sem->value ++; /* increase value */
        }
        else
        {
            rt_hw_interrupt_enable(temp); /* enable interrupt */
            return -RT_EFULL; /* value overflowed */
        }
    }

```



#### 6.2.3 获取信号量

调用过程如下：

```c
rt_sem_take(rt_sem_t sem, rt_int32_t time)
    /* 简单粗暴: 关中断 */
	/* disable interrupt */
    temp = rt_hw_interrupt_disable();

    /* 有资源? 减1、返回 */
    if (sem->value > 0)
    {
        /* semaphore is available */
        sem->value --;

        /* enable interrupt */
        rt_hw_interrupt_enable(temp);
    }
    else /* 没有资源? */
    {
        	/*省略其他代码*/
        
        	/* 当前线程: 1.休眠, 2.把自己记录在信号量的parent.suspend_thread链表里 */
            /* suspend thread */
            rt_ipc_list_suspend(&(sem->parent.suspend_thread),
                                thread,
                                sem->parent.parent.flag);

            /* 让出处理器 */
	        /* do schedule */
            rt_schedule();

        	/* 这个线程能再次运行时: 是因为超时?  */
            if (thread->error != RT_EOK)
            {
                /* 直接返回错误 */
                return thread->error;
            }
    }

	/* 这个线程能再次运行时: 不是是因为超时? 返回OK  */   
    return RT_EOK;

```



## 7. 互斥量(mutex)

互斥量是特殊的信号量：

* 资源个数：只能是0、1
* 实现了优先级继承



### 7.1 优先级反转与继承



### 7.2  核心是：链表、定时器、优先级

只有1个链表：

* 等待互斥量不成功而挂起
* 释放互斥量：无需挂起

定时器：

* 在线程结构体里面
* 用来实现超时机制

优先级：实现优先级继承。



![image-20220122083240529](pic/mutex/02_struct.png)

### 7.3 操作示例

示例程序：`RT-Thread_13_mutex`

#### 7.3.1 创建互斥量

![image-20220122082448980](pic/mutex/01_create.png)

#### 7.3.2 获取互斥量

视频里讲解，比较复杂。



#### 7.3.3 释放互斥量

视频里讲解，比较复杂。





## 8. 事件组(event group)

队列：用来传递数据，大小不限

邮箱：用来传递数据，数据是一个整数

信号量：表示资源数量

互斥量：实现互斥操作，跟信号量的主要区别在于实现了优先级继承

事件组：实现"等待多个事件"

适应场景：

学校组织秋游，组长在等待：

* 张三：我到了
* 李四：我到了
* 王五：我到了
* 组长说：好，大家都到齐了，出发！

秋游回来第二天就要提交一篇心得报告，组长在焦急等待：张三、李四、王五谁先写好就交谁的。

在这个日常生活场景中：

* 出发：要等待这3个人都到齐，他们是"与"的关系
* 交报告：只需等待这3人中的任何一个，他们是"或"的关系



事件组：

![image-20220122121924963](pic/event_groutp/02_event_group.png)



### 8.1 核心：链表、定时器、事件组合

#### 8.1.1 rt_event结构体

rt_event结构体里表示：当前发生了哪些事件。

![image-20220122121612686](pic/event_groutp/01_struct.png)

#### 8.1.2 rt_thread结构体

每个线程都有一个rt_thread结构体，里面表明了：

* 它期待哪些事件
* 它是期待这些事件的：所有？任意一个？

![image-20220122123503533](pic/event_groutp/03_event_in_thread.png)



### 8.2 操作示例

示例程序：`RT-Thread_15_event_group_wait_multi_events`

#### 8.2.1 创建事件组

![image-20220122122727703](pic/event_groutp/04_event_create.png)





#### 8.2.2 发出事件

**注意**：发出事件时试图唤醒**所有**符合条件的线程。写队列、写邮箱、释放信号量、释放互斥量时，都只会唤醒**一个**线程。

调用关系如下：

```c
rt_event_send(rt_event_t event, rt_uint32_t set)
    /* disable interrupt */
    level = rt_hw_interrupt_disable();

    /* set event */
    event->set |= set;
    /* 更复杂的调用关系看下图: */
```

![image-20220122123219100](pic/event_groutp/05_rt_event_send.png)





#### 8.2.3 等待事件

rt_event_recv函数：

* 期待的事件未发生：可以直接返回，也可以等待
  * 如果要等待的话，等待的细节记录在rt_thread结构体里
* 期待的事件发生了：
  * 根据option决定是否清除事件组

```c
rt_err_t rt_event_recv(rt_event_t   event,
                       rt_uint32_t  set,
                       rt_uint8_t   option,
                       rt_int32_t   timeout,
                       rt_uint32_t *recved);
```





## 9. 信号(signal)

信号，不是信号量(semaphore)。

这不是同一个东西。

### 9.1 妈妈怎么知道孩子醒了

![image-20220122083826518](pic/signal/01_mom.png)

妈妈怎么知道卧室里小孩醒了？

① 时不时进房间看一下：查询方式

简单，但是累

② 进去房间陪小孩一起睡觉，小孩醒了会吵醒她：休眠-唤醒

不累，但是妈妈干不了活了

③ 妈妈要干很多活，但是可以陪小孩睡一会，定个闹钟：加超时

要浪费点时间，但是可以继续干活。

妈妈要么是被小孩吵醒，要么是被闹钟吵醒。

④ 妈妈在客厅干活，小孩醒了他会自己走出房门告诉妈妈：异步通知，就是发信号

妈妈、小孩互不耽误。



### 9.2 信号处理流程

示例程序：`RT-Thread_11_signals`

信号就是线程的"软件中断"，跟"硬件中断"类似：

* 要安装信号处理函数，相当于给硬件中断提供处理函数

  ```c
  
      /* thread1: 安装信号,自定义处理函数 */
  	rt_signal_install(SIGUSR1, thread1_signal_handler);
  ```

  

* 要使能信号，相当于使能硬件中断

  ```c
  	/* thread1: 解除屏蔽 */
  	rt_signal_unmask(SIGUSR1);
  ```

  

* 发出信号，相当于触发硬件中断

  ```c
      rt_thread_kill(thread1, SIGUSR1); //向线程1发送信号SIGUSR1
  ```



#### 9.2.1 给自己发信号

函数调用过程：

```c
rt_thread_kill
    _signal_deliver
    	rt_thread_handle_sig(RT_TRUE);
			handler = tid->sig_vectors[signo];
			if (handler) handler(signo);
```



![image-20220122124447228](pic/event_groutp/06_send_signal_to_myself.png)





#### 9.2.2 给阻塞态的线程发信号

会唤醒这个阻塞态的线程，这个线程运行时先处理信号。

发信号：

```
rt_thread_kill
    _signal_deliver
```

![image-20220122125134648](pic/event_groutp/07_send_signal_to_blocked_thread.png)



线程被唤醒后：

![image-20220122125450874](pic/event_groutp/08_rt_schedule.png)





#### 9.2.3 给就绪态的线程发信号

这种情况最复杂。

假设线程A给线程B发信号，线程B的优先级比较低：

* 现在是线程A在运行，线程B的运行状态保存在线程B的栈里：比栈里PC值等于xxx，表示线程B继续运行时从xxx运行
* 即使线程B收到了信号，它也不能马上执行信号处理函数
* 怎样保证：线程B能运行时，先运行信号处理函数呢？

**重点**：线程A发信号给线程B时，如果线程B是就绪态，就**修改线程B**的栈，新增加一个"虚构的现场"，里面保存有"信号处理函数的地址"。

函数调用关系：

```c
rt_thread_kill
    _signal_deliver
```

![image-20220122130336688](pic/event_groutp/09_send_signal_to_running_task.png)



当线程B再次运行时，先运行`_signal_entry`函数：

![image-20220122130650138](pic/event_groutp/10_signal_entry.png)





## 10. ARM架构与RT-Thread





## 10. 引脚驱动程序

参考文档：`https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/programming-manual/device/device`

参考代码：

```shell
rt-thread-v3.1.5\bsp\stm32\libraries\HAL_Drivers\drv_gpio.c
rt-thread-v3.1.5\components\drivers\misc\pin.c
```





### 10.1 驱动框架


RT-Thread中，所有的设备驱动都使用一个结构体`rt_device`来表示:

![image-20220122133813112](pic/event_groutp/12_rt_device.png)





这些结构体都保存在一个链表中：

![image-20220122134314026](pic/event_groutp/11_rt_object_container.png)



要去使用这个驱动，就是先在上述链表里使用名字找到对应的rt_device结构体，然后调用里面的init/open/read/write/control/close等函数。

* 找到：rt_deivce_find
* 操作：如下图

![image-20220122134744865](pic/event_groutp/13_rt_dev_opration.png)

示例代码：`rt-thread-v3.1.5\examples\test\device_test.c`

```c
rt_device_t device = RT_NULL;

// step 1:find device
device = rt_device_find(device_name);

// step 2:init device
if (!(device->flag & RT_DEVICE_FLAG_ACTIVATED))
{
    rt_err_t result;
    result = rt_device_init(device);
    if (result != RT_EOK)
    {
        rt_kprintf("To initialize device:%s failed. The error code is %d\r\n",
                   device->parent.name, result);
        return result;
    }
    else
    {
        device->flag |= RT_DEVICE_FLAG_ACTIVATED;
    }
}

// open
result = rt_device_open(device,RT_DEVICE_FLAG_RDWR);

// control
result = rt_device_control(device,
                           RT_DEVICE_CTRL_BLK_GETGEOME,
                           &geometry);

// read/write
i = rt_device_read(device, 0, read_buffer, 1);
i = rt_device_write(device, sector_no, write_buffer,1);

```




### 10.2 注册pin驱动

```c
    return rt_device_pin_register("pin", &_stm32_pin_ops, RT_NULL);
```



### 10.3 使用过程





## 11. 按键驱动程序





## 12. LCD驱动程序





## 13. I2C驱动程序





## 14. SPI驱动程序





