#ifndef ISR_H
#define ISR_H

#include <core/common.h>

#define PIC_MASTER_PORT_A 0x20
#define PIC_MASTER_PORT_B 0x21
#define PIC_SLAVE_PORT_A 0xA0
#define PIC_SLAVE_PORT_B 0xA1
#define PIC_INTERRUPT_ACK 0x20

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

typedef struct registers
{
	u32 ds;
	u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
	u32 int_no, err_code;
	u32 eip, cs, eflags, useresp, ss;
} registers_t;

typedef void (*isr_t)(registers_t*);
void register_interrupt_handler(u8 n, isr_t handler);

#endif // ISR_H
