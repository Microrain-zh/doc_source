
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <poll.h>
#include <signal.h>
#include <unistd.h>


/*
 * ./ds18b20_test /dev/myds18b20
 *
 */
int main(int argc, char **argv)
{
	int fd;
	unsigned char data[5];
	unsigned int integer;
	unsigned char decimal;

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
		if (read(fd, data, 5) == 5)
		{
			integer = data[0] | (data[1]<<8) | (data[2]<<16) | (data[3]<<24);
			decimal = data[4];
			
			printf("get temprature: %d.%d\n", integer, decimal);
		}
		else 
		{
			printf("get temprature: -1\n");
		}
		sleep(1);
	}
	
	close(fd);
	
	return 0;
}


