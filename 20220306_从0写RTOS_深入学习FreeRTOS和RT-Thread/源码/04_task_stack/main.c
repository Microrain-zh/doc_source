
#include "uart.h"
#include "string.h"


void task_a(char *str)
{
	char buf[10] = "task a";
	volatile int i;
	
	puts(buf);
	while (1)
	{
		i= 0;
		while (str[i])
			putchar(str[i]);
		i++;
	}
}

void task_b(char *str)
{
	char buf[10] = "task b";
	volatile int i;
	
	puts(buf);
	while (1)
	{
		i= 0;
		while (str[i])
			putchar(str[i]);
		i++;
	}
}

int mymain()
{
	while (1)
	{
		task_a("www.100ask.net");
		task_b("rtos");
	}
	return 0;
}
