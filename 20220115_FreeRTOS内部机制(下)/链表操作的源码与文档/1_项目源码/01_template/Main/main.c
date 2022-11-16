#include "usart.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char **arg)
{
    char *p;
    
    HAL_Init();
    
    MX_USART1_UART_Init();
    
    if(p==NULL)
    {
        p = (char*)malloc(sizeof(char)*10);
    }
    
    memset(p, 0xFF, sizeof(char)*10);
    
    if(p != NULL)
    {
        free(p);
    }
    
    printf("Malloc\r\n");
    
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
