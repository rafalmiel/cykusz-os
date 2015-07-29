#include <core/common.h>
#include <core/io.h>

#include "common.h"
#include "isr.h"
#include "localapic.h"

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
	kprint("IRQ HANDLER");
	//kprint_hexnl(reg->int_no);
	/*if (reg->int_no >= 40) {
		outb(PIC_MASTER_PORT_B, PIC_INTERRUPT_ACK);
	}

	outb(PIC_MASTER_PORT_A, PIC_INTERRUPT_ACK);*/

	if (interrupt_handlers[reg->int_no] != 0) {
		isr_t handler = interrupt_handlers[reg->int_no];
		handler(reg);
	}

	lapic_end_of_interrupt();
}

void register_interrupt_handler(u8 n, isr_t handler)
{
	interrupt_handlers[n] = handler;
}
