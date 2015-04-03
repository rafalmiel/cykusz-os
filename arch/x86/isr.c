#include <core/common.h>
#include <core/io.h>

#include "common.h"
#include "isr.h"

isr_t interrupt_handlers[256];

void isr_handler(registers_t *reg)
{
	if (interrupt_handlers[reg->int_no] != 0) {
		isr_t handler = interrupt_handlers[reg->int_no];
		handler(reg);
	} else {
		kprint("unhandled interrupt: ");
		kprint_intnl(reg->int_no);

		asm volatile("hlt");
	}

}


void irq_handler(registers_t *reg)
{
	if (reg->int_no >= 40) {
		outb(0xA0, 0x20);
	}

	outb(0x20, 0x20);

	if (interrupt_handlers[reg->int_no] != 0) {
		isr_t handler = interrupt_handlers[reg->int_no];
		handler(reg);
	}
}

void register_interrupt_handler(u8 n, isr_t handler)
{
	interrupt_handlers[n] = handler;
}
