#ifndef _TASK_H
#define _TASK_H

typedef void (*task_function)(void *param);

int get_stack(int task_index);
void create_task(task_function f, void *param, char *stack, int stack_len);
void start_task(void);
int is_task_running(void);

void StartTask_asm(int sp, int lr);

extern int cur_task;

#endif
