
#include "uart.h"
#include "string.h"


int add_val(int a, int b, int c, int d)
{
	// 故意使用R4
	register int sum asm("r4");
	
	sum = a+b+c+d;
	
	return sum;
}


int mymain()
{
	volatile int a = 1;
	volatile int b = 2;
	volatile int c = 3;
	volatile int d = 4;
	volatile int sum;
	
	sum = add_val(a,b,c,d);
	
	return 0;
}
