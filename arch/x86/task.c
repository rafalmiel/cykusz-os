#include <core/io.h>
#include <core/kheap.h>

#include "task.h"

#define STACK_SIZE 4096

task_t *tsk1;
task_t *tsk2;

static void task2()
{
	while (1) {
		kprint("I AM TASK 2\r");
		swtch(&tsk2->context, tsk1->context);
	}
}

void init_tasking()
{
	u8 *sp;

	tsk1 = (task_t*)kmalloc(sizeof *tsk1);
	tsk2 = (task_t*)kmalloc(sizeof *tsk2);

	kprint("ESP: ");
	kprint_hexnl(read_esp());

	sp = (u8*)(kmalloc_a(STACK_SIZE) + STACK_SIZE);
	sp -= sizeof *tsk2->context;
	tsk2->context = (context_t *)sp;
	memset(tsk2->context, 0, sizeof *tsk2->context);
	tsk2->context->eip = (u32)task2;

	tsk1->context = 0;
	while (1) {
		kprint("I AM TASK 1\r");
		swtch(&tsk1->context, tsk2->context);
	}
}
