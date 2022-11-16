
#define TASK_COUNT 32

typedef void (*task_function)(void *param);

static int task_stacks[TASK_COUNT];
static int task_count;

void create_task(task_function f, void *param, char *stack, int stack_len)
{
	int *top = stack + stack_len;
	
	/* 伪造现场 */
	top -= 16;
	
	/* r4~r11 */
	top[0] = 0; /* r4 */
	top[1] = 0; /* r5 */
	top[2] = 0; /* r6 */
	top[3] = 0; /* r7 */
	top[4] = 0; /* r8 */
	top[5] = 0; /* r9 */
	top[6] = 0; /* r10 */
	top[7] = 0; /* r11 */
	
	/* r0~r3 */
	top[8]  = param; /* r0 */
	top[9]  = 0; /* r1 */
	top[10] = 0; /* r2 */
	top[11] = 0; /* r3 */
	
	/* r12,lr */
	top[12] = 0; /* r12 */
	top[13] = 0; /* lr */
	
	/* 返回地址 */
	top[14] = f; /* 任务入口 */
	
	/* PSR */
	top[15] = (1<<24); /* psr 使用thumb指令集 */	
	
	/* 记录栈的位置 */
	task_stacks[task_count++] = (int)top;
}
