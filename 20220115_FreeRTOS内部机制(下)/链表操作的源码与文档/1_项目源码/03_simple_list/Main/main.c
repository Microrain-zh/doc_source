#include "usart.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct person {
	char *name;
	int age;
	int sex;
	struct person *next;
};

struct list {
	char *name; /* A班 */
	struct person *next;
};

int main(int argc, char **arg)
{
	struct list a_list;

	struct person p1;
	
	
	a_list.name = "A";
	a_list.next = NULL;  /* 空链表 */
	
	p1.name = "a1";
	p1.next = NULL;
	
	a_list.next = &p1;
	
    
    HAL_Init();
    
    MX_USART1_UART_Init();
	
	printf("a list: %s\r\n", a_list.next->name);
    
    while(1)
    {
    }
}

void Error_Handler(void)
{
    printf("Error\r\n");
    while(1)
    {
    }
}
