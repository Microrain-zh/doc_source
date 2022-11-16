
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

/*
 * ./hello_drv_test -w abc
 * ./hello_drv_test -r
 */
int main(int argc, char **argv)
{
	int fd;
	char buf[1024];
	int len;
	int ret;
	
	/* 1. 判断参数 */
	if (argc < 2) 
	{
		printf("Usage: %s -w <string>\n", argv[0]);
		printf("       %s -r\n", argv[0]);
		return -1;
	}

	/* 2. 打开文件 */
	fd = open("/dev/xyz", O_RDWR);
	if (fd == -1)
	{
		printf("can not open file /dev/xyz\n");
		return -1;
	}
	
	printf("open file /dev/xyz ok\n");

	/* 3. 写文件或读文件 */
	if ((0 == strcmp(argv[1], "-w")) && (argc == 3))
	{
		len = strlen(argv[2]) + 1;
		len = len < 1024 ? len : 1024;
		ret = write(fd, argv[2], len);
		printf("write driver: %d\n", ret);
	}
	else
	{
		len = read(fd, buf, 1024);		
		printf("read driver: %d\n", len);
		buf[1023] = '\0';
		printf("APP read : %s\n", buf);
	}
	
	close(fd);
	
	return 0;
}


