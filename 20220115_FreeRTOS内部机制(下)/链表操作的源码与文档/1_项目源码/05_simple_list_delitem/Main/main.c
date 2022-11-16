#include "usart.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct person {
	char *name;
	int age;
	struct person *next;
};

struct list {
	char *name; /* A班 */
	struct person *next;
};

void InitList(struct list *pList, char *name)
{
	pList->name = name;
	pList->next = NULL;
}

void AddItemToList(struct list *pList, struct person *new_person)
{
	struct person *last;
	
	/* 如果是空链表 */
	if (pList->next == NULL)
	{
		pList->next = new_person;
		new_person->next =NULL;
		return;
	}
	
	last = pList->next;
	while (last->next)
	{
		last = last->next;
	}
	
	/* last->next == NULL */
	last->next = new_person;
	new_person->next =NULL;
}

void DelItemFromList(struct list *pList, struct person *person)
{
	struct person *p = pList->next;
	struct person *pre = NULL;
	
	/* 找到person */
	while (p != NULL && p != person)
	{
		/* 后面还有人, 移动到下一个 */
		pre = p;
		p = p->next;
	}
	
	/* 退出的条件: p==NULL, p == person */
	if (p == NULL)
	{
		printf("can not find the person to del\r\n");
		return;
	}
	
	if (pre == NULL) /* 前面无人, 表示要删除的是第1项 */
	{
		pList->next = p->next;
	}
	else
	{
		pre->next = p->next;
	}	
}


void PrintList(struct list *pList)
{
	int i = 0;
	
	struct person *p = pList->next;
	
	while (p != NULL)
	{
		printf("person %d: %s is %d\r\n", i++, p->name, p->age);
		
		/* 后面还有人, 移动到下一个 */
		p = p->next;
	}
}

int main(int argc, char **arg)
{
	struct list a_list;
	int i;

	struct person p[] = {
		{"p1", 10, NULL},
		{"p2", 20, NULL},
		{"p3", 13, NULL},
		{"p4", 41, NULL},
		{"p5", 56, NULL},
		{"p6", 12, NULL},
		{"p7", 9, NULL},
		{"p8", 21, NULL},
		{NULL, 0, NULL},
	};
	
	
    HAL_Init();
    
    MX_USART1_UART_Init();

	InitList(&a_list, "A_class");

	i = 0;
	while (p[i].name != NULL)
	{
		AddItemToList(&a_list, &p[i]);
		i++;
	}

	printf("add all person:\r\n");
	PrintList(&a_list);
	
	DelItemFromList(&a_list, &p[3]);

	printf("del person %s:\r\n", p[3].name);
	PrintList(&a_list);

	DelItemFromList(&a_list, &p[0]);
	
	printf("del person %s:\r\n", p[0].name);
	PrintList(&a_list);
    
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
