
#include "uart.h"
#include "string.h"

static char stack_a[1024];
static char stack_b[1024];

void task_a(void *param)
{
	char c = (char)param;
	while (1)
	{
		putchar(c);
	}
}

int mymain()
{
	create_task(task_a, 'a', stack_a, 1024);
	create_task(task_a, 'b', stack_b, 1024);
	return 0;
}
