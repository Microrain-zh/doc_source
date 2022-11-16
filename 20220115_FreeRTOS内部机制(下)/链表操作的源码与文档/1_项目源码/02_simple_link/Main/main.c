#include "usart.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct person {
	char *name;
	int age;
	int sex;
	struct person *couple;
};


struct person w;
struct person h;

int main(int argc, char **arg)
{
    
    HAL_Init();
    
    MX_USART1_UART_Init();
	
	w.name = "w";
	w.age  = 30;
	w.couple = &h;

	h.name = "h";
	h.age  = 30;
	h.couple = &w;
    
	printf("w's couple is %s\r\n", w.couple->name);
	
    
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
