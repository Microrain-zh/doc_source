/*  Copyright (s) 2019 深圳百问网科技有限公司
 *  All rights reserved
 * 
 * 文件名称：ring_buffer.c
 * 摘要：
 *  
 * 修改历史     版本号        Author       修改内容
 *--------------------------------------------------
 * 2021.8.21      v01         百问科技      创建文件
 *--------------------------------------------------
*/

#include "ring_buffer.h"


/*
 *  函数名：void ring_buffer_init(ring_buffer *dst_buf)
 *  输入参数：dst_buf --> 指向目标缓冲区
 *  输出参数：无
 *  返回值：无
 *  函数作用：初始化缓冲区
*/
void ring_buffer_init(ring_buffer *dst_buf)
{
    dst_buf->pW = 0;
    dst_buf->pR = 0;
}

/*
 *  函数名：void ring_buffer_write(unsigned char c, ring_buffer *dst_buf)
 *  输入参数：c --> 要写入的数据
 *            dst_buf --> 指向目标缓冲区
 *  输出参数：无
 *  返回值：无
 *  函数作用：向目标缓冲区写入一个字节的数据，如果缓冲区满了就丢掉此数据
*/
void ring_buffer_write(unsigned char c, ring_buffer *dst_buf)
{
    int i = (dst_buf->pW + 1) % BUFFER_SIZE;
    if(i != dst_buf->pR)    // 环形缓冲区没有写满
    {
        dst_buf->buffer[dst_buf->pW] = c;
        dst_buf->pW = i;
    }
}

/*
 *  函数名：int ring_buffer_read(unsigned char *c, ring_buffer *dst_buf)
 *  输入参数：c --> 指向将读到的数据保存到内存中的地址
 *            dst_buf --> 指向目标缓冲区
 *  输出参数：无
 *  返回值：读到数据返回0，否则返回-1
 *  函数作用：从目标缓冲区读取一个字节的数据，如果缓冲区空了返回-1表明读取失败
*/
int ring_buffer_read(unsigned char *c, ring_buffer *dst_buf)
{
    if(dst_buf->pR == dst_buf->pW)
    {
        return -1;
    }
    else
    {
        *c = dst_buf->buffer[dst_buf->pR];
        dst_buf->pR = (dst_buf->pR + 1) % BUFFER_SIZE;
        return 0;
    }
}