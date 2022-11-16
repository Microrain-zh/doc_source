
#include "uart.h"
#include "string.h"


int add_val(int *pa, int *pb)
{
	volatile int tmp;
	tmp = *pa;
	tmp += *pb;
	return tmp;
}


int mymain()
{
	volatile int a = 1;
	volatile int b = 2;
	volatile int c;
	
	c = add_val(&a, &b);
	
	return 0;
}
