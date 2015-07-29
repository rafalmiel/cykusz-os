#include <core/timer.h>
#include <core/io.h>

#include "common.h"
#include "isr.h"
#include "ioapic.h"
#include "acpi.h"

static u32 tick = 0;

static void timer_callback(registers_t *regs)
{
	(void) regs;

	tick++;
	kprint("Tick: ");
	kprint_int(regs->int_no);
	kprint(" ");
	kprint_int(tick);
	kprint("\r");
}

void init_timer_divisor(u32 div)
{
	register_interrupt_handler(IRQ0, &timer_callback);

	u16 divisor = div;

	outb(0x43, 0x36);

	u8 l = (u8)(divisor & 0xFF);
	u8 h = (u8)((divisor >> 8) & 0xFF);

	outb(0x40, h);
	outb(0x40, l);

	ioapic_set_int(acpi_remap_irq(0), 32);
}

void init_timer(u32 ms)
{
	register_interrupt_handler(IRQ0, &timer_callback);

	u16 hz = 1000 / ms;

	u16 divisor = 1193182 / hz;

	kprint_intnl(hz);
	kprint_intnl(divisor);

	outb(0x43, 0x36);

	u8 l = (u8)(divisor & 0xFF);
	u8 h = (u8)((divisor >> 8) & 0xFF);

	outb(0x40, h);
	outb(0x40, l);

	ioapic_set_int(acpi_remap_irq(0), 32);
}
