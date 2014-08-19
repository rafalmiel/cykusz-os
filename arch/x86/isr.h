#ifndef ISR_H
#define ISR_H

#include <core/common.h>

typedef struct registers
{
	u32 ds;
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32 int_no, err_code;
	u32 eip, cs, eflags, useresp, ss;
} registers_t;

#endif // ISR_H
