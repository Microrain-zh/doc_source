
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <signal.h>


/*
 * ./sr04_test /dev/sr04
 *
 */
int main(int argc, char **argv)
{
	int fd;
	int us;

	int i;
	
	/* 1. 判断参数 */
	if (argc != 2) 
	{
		printf("Usage: %s <dev>\n", argv[0]);
		return -1;
	}


	/* 2. 打开文件 */
//	fd = open(argv[1], O_RDWR | O_NONBLOCK);
	fd = open(argv[1], O_RDWR);
	if (fd == -1)
	{
		printf("can not open file %s\n", argv[1]);
		return -1;
	}


	while (1)
	{
		if (read(fd, &us, 4) == 4)
		{
			printf("get distance: %d us\n", us);  /* mm */
			printf("get distance: %d mm\n", us*340/2/1000);  /* mm */
		}
		else
			printf("get distance: -1\n");
	}
	
	close(fd);
	
	return 0;
}


