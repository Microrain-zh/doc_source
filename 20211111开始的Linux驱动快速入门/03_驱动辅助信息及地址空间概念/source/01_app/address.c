
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int a;

/* ./address 123 
 *  argv[0] = "./address"
 *  argv[1] = "123"
 */

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: %s <val>\n", argv[0]);
		return -1;
	}

	a = strtol(argv[1], NULL, 0);
	printf("a = %d\n", a);
	printf("a's address = 0x%lx\n", (unsigned long)&a);
	printf("main's address = 0x%lx\n", (unsigned long)main);
	
	while (1)
	{
		sleep(100);
	}
	return 0;
}

