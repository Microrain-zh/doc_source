#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include <asm/io.h>

#include "led_opr.h"

/* registers */
// RCC_PLL4CR地址：0x50000000 + 0x894
static volatile unsigned int *RCC_PLL4CR;

// RCC_MP_AHB4ENSETR 地址：0x50000000 + 0xA28
static volatile unsigned int *RCC_MP_AHB4ENSETR;

// GPIOA_MODER 地址：0x50002000 + 0x00
static volatile unsigned int *GPIOA_MODER;

// GPIOA_BSRR 地址： 0x50002000 + 0x18
static volatile unsigned int *GPIOA_BSRR;


static int board_demo_led_init (int which) /* 初始化LED, which-哪个LED */	   
{

	if (!RCC_PLL4CR)
	{
		// RCC_PLL4CR地址：0x50000000 + 0x894
		RCC_PLL4CR = ioremap(0x50000000 + 0x894, 4);
		
		// RCC_MP_AHB4ENSETR 地址：0x50000000 + 0xA28
		RCC_MP_AHB4ENSETR = ioremap(0x50000000 + 0xA28, 4);
		
		// GPIOA_MODER 地址：0x50002000 + 0x00
		GPIOA_MODER = ioremap(0x50002000 + 0x00, 4);
		
		// GPIOA_BSRR 地址： 0x50002000 + 0x18
		GPIOA_BSRR = ioremap(0x50002000 + 0x18, 4);
	}

	if (which == 0)
	{
		/* enalbe PLL4, it is clock source for all gpio */
		*RCC_PLL4CR |= (1<<0);
		while ((*RCC_PLL4CR & (1<<1)) == 0);
		
		/* enable gpioA */
		*RCC_MP_AHB4ENSETR |= (1<<0);
		
		/*
		 * configure gpa10 as gpio
		 * configure gpio as output 
		 */
		*GPIOA_MODER &= ~(3<<20);
		*GPIOA_MODER |= (1<<20);
	}

	return 0;
}

static int board_demo_led_ctl (int which, char status) /* 控制LED, which-哪个LED, status:1-亮,0-灭 */
{
	if (which == 0)
	{
		/* to set gpio register: out 1/0 */
		if (status)
		{
			/* set gpa10 to let led on */
			*GPIOA_BSRR = (1<<26);
		}
		else
		{
			/* set gpa10 to let led off */
			*GPIOA_BSRR = (1<<10);
		}
	}
	return 0;
}

static struct led_operations board_demo_led_opr = {
	.num  = 1,
	.init = board_demo_led_init,
	.ctl  = board_demo_led_ctl,
};

struct led_operations *get_board_led_opr(void)
{
	return &board_demo_led_opr;
}


