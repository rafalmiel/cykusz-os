#ifndef TASK_H
#define TASK_H

#include <core/common.h>

typedef struct stack {
	u32 eflags;
	u32 edi;
	u32 esi;
	u32 ebx;
	u32 ebp;
	u32 eip;
} context_t;

typedef struct task {
	context_t *context;
} task_t;

extern void swtch(context_t **old_ctx, context_t *new_ctx);
extern u32 read_esp(void);

void init_tasking();

#endif // TASK_H

