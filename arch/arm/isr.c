#include <core/io.h>

#include "isr.h"
#include "gpio.h"
#include "timer.h"

#define __interrupt(name) __attribute__((interrupt(name)))

#define RPI_INTERRUPT_CONTROLLER_BASE   0x2000B200

/* The interrupt controller memory mapped register set */
typedef struct {
	volatile u32 irq_basic_pending;
	volatile u32 irq_pending_1;
	volatile u32 irq_pending_2;
	volatile u32 fiq_control;
	volatile u32 enable_irqs_1;
	volatile u32 enable_irqs_2;
	volatile u32 enable_basic_irqs;
	volatile u32 disable_irqs_1;
	volatile u32 disable_irqs_2;
	volatile u32 disable_basic_irqs;
} rpi_irq_controller_t;

/* The BCM2835 Interupt controller peripheral at it's base address */
static rpi_irq_controller_t* rpi_controller =
		(rpi_irq_controller_t*)RPI_INTERRUPT_CONTROLLER_BASE;

__attribute__((naked, aligned(32))) static void isr_vectors(void)
{
	asm volatile("b _start\n"
		     "b int_undefined\n"
		     "b int_software\n"
		     "b int_prefetch_abort\n"
		     "b int_data_abort\n"
		     "nop\n"
		     "b int_interrupt\n"
		     "b int_fast_interrupt");
}

void init_interrupts()
{
	/* Set interrupt base register */
	asm volatile("mcr p15, 0, %[addr], c12, c0, 0" : : [addr] "r" (&isr_vectors));
	/* Turn on interrupts */
	asm volatile("cpsie i");
}

void rpi_basic_interrupt_enable(u32 source)
{
	rpi_controller->enable_basic_irqs = source;
}

void __interrupt("UNDEF") int_undefined(void)
{
	kprint("Got UNDEF interrupt\n");
	while (1)
	{

	}
}

void __interrupt("SWI") int_software(void)
{
}

void __interrupt("ABORT") int_prefetch_abort(void)
{

}

void __interrupt("ABORT") int_data_abort(void)
{

}

static int states[] = {1, 3, 2, 0};

void __interrupt("IRQ") int_interrupt(void)
{
	static int ind = 0;

	kprint("Timer!");

	rpi_timer_irqclear();

	ind = (ind + 1) % 4;

	int off = states[ind];

	if (off & 0x1)
		gpio_actled_on();
	else
		gpio_actled_off();

	if (off & 0x2)
		gpio_pwrled_on();
	else
		gpio_pwrled_off();
}

void __interrupt("FIQ") int_fast_interrupt(void)
{
	while (1) {}
}
