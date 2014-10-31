#include <core/timer.h>
#include "isr.h"
#include "vga.h"

static u32 tick = 0;

static void timer_callback(registers_t *regs)
{
	(void) regs;

	tick++;
	vga_writestring("Tick: ");
	kprint_int(regs->int_no);
	vga_writestring(" ");
	kprint_int(tick);
	vga_writestring("\n");
}

void init_timer(u32 freq)
{
	register_interrupt_handler(IRQ0, &timer_callback);

	u32 divisor = 1193180 / freq;

	outb(0x43, 0x36);

	u8 l = (u8)(divisor & 0xFF);
	u8 h = (u8)((divisor >> 8) & 0xFF);

	outb(0x40, h);
	outb(0x40, l);
}
