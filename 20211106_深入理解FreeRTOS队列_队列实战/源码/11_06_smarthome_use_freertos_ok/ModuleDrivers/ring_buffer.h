#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

#include "stm32f1xx_hal.h"

#define BUFFER_SIZE 128        /* 环形缓冲区的大小 */
typedef struct
{
    unsigned char buffer[BUFFER_SIZE];  /* 缓冲区空间 */
    volatile unsigned int pW;           /* 写地址 */
    volatile unsigned int pR;           /* 读地址 */
} ring_buffer;

extern void ring_buffer_init(ring_buffer *dst_buf);
extern void ring_buffer_write(unsigned char c, ring_buffer *dst_buf);
extern int ring_buffer_read(unsigned char *c, ring_buffer *dst_buf);

#endif /* __RING_BUFFER_H */

