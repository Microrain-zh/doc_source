#include "usart.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct node_t {
	struct node_t *next;
};

struct person {
	char *name;
	int age;
	struct node_t node;
};

struct list {
	char *name; /* A班 */
	struct node_t head;
};

void InitList(struct list *pList, char *name)
{
	pList->name = name;
	pList->head.next = NULL;
}

void AddItemToList(struct list *pList, struct node_t *new_node)
{
	struct node_t *last = &pList->head;
	
	while (last->next)
	{
		last = last->next;
	}
	
	/* last->next == NULL */
	last->next = new_node;
	new_node->next =NULL;
}

void AddItemAfter(struct node_t *pre, struct node_t *new_node)
{
	new_node->next = pre->next;
	pre->next = new_node;
}

void DelItemFromList(struct list *pList, struct node_t *node)
{
	struct node_t *pre = &pList->head;
	
	/* 找到node */
	while (pre != NULL && pre->next != node)
	{
		pre = pre->next;
	}
	
	/* 没找到 */
	if (pre == NULL)
		return;
	else
		pre->next = node->next;
}

int CmpPersonAge(struct node_t *pre, struct node_t *next)
{
	struct person *p;
	struct person *n;
	
	p = (struct person *)((char *)pre - (unsigned int)&((struct person *)0)->node);
	n = (struct person *)((char *)next - (unsigned int)&((struct person *)0)->node);
	
	if (p->age < n->age)
		return -1;
	else
		return 0;
}

void SortList(struct list *pList)
{
	struct node_t *pre1 = &pList->head;
	struct node_t *pre2;
	struct node_t *cur = pre1->next;
	struct node_t *next;
	struct node_t *tmp;
		
	while (cur)
	{
		pre2 = cur;
		next = cur->next;
		while (next)
		{
			if (CmpPersonAge(cur, next) == 0)
			{
				/* 交换节点 */
				/* 1. del cur */
				DelItemFromList(pList, cur);
				
				/* 2. del next */
				DelItemFromList(pList, next);
				
				/* 3. 在pre1之后插入next */
				AddItemAfter(pre1, next);
				
				/* 4. 在pre2之后插入cur */
				if (pre2 == cur)
					AddItemAfter(next, cur);
				else
					AddItemAfter(pre2, cur);
				
				/* 5. cur/next指针互换 */
				tmp = cur;
				cur = next;
				next = tmp;				
			}
			
			pre2 = next;
			next = next->next;
		}
		
		pre1 = cur;
		cur = cur->next;
	}
	
}

void PrintList(struct list *pList)
{
	int i = 0;
	
	struct node_t *node = pList->head.next;
	struct person *p;
	
	while (node != NULL)
	{
		p = (struct person *)((char *)node - (unsigned int)&((struct person *)0)->node);;
		printf("person %d: %s is %d\r\n", i++, p->name, p->age);
		
		/* 后面还有人, 移动到下一个 */
		node = node->next;
	}
}

int main(int argc, char **arg)
{
	struct list a_list;
	int i;

	struct person p[] = {
		{"p1", 10, {NULL}},
		{"p2", 20, {NULL}},
		{"p3", 13, {NULL}},
		{"p4", 41, {NULL}},
		{"p5", 56, {NULL}},
		{"p6", 12, {NULL}},
		{"p7", 9, {NULL}},
		{"p8", 21, {NULL}},
		{"p9", 22, {NULL}},
		{"p10", 21, {NULL}},
		{"p11", 20, {NULL}},
		{NULL, 0, {NULL}},
	};
	
	
    HAL_Init();
    
    MX_USART1_UART_Init();

	InitList(&a_list, "A_class");

	i = 0;
	while (p[i].name != NULL)
	{
		AddItemToList(&a_list, &p[i].node);
		i++;
	}

	printf("add all person:\r\n");
	PrintList(&a_list);
	
	DelItemFromList(&a_list, &p[3].node);

	printf("del person %s:\r\n", p[3].name);
	PrintList(&a_list);

	DelItemFromList(&a_list, &p[0].node);
	
	printf("del person %s:\r\n", p[0].name);
	PrintList(&a_list);
	
	SortList(&a_list);
	printf("sort list, all person:\r\n");
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
