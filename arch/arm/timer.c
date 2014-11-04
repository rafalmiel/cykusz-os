#include <core/timer.h>

#include "ioports.h"
#include "timer.h"
#include "isr.h"

/* See the documentation for the ARM side timer (Section 14 of the
   BCM2835 Peripherals PDF) */
#define RPI_ARMTIMER_BASE		(IO_BASE + 0x0000B400)

/* Section 14.2 of the BCM2835 Peripherals documentation details
   the register layout for the ARM side timer */
typedef struct {
	volatile u32 load;
	volatile u32 value;
	volatile u32 control;
	volatile u32 irqclear;
	volatile u32 rawirq;
	volatile u32 maskedirq;
	volatile u32 reload;
	volatile u32 predivider;
	volatile u32 freerunningcounter;

} rpi_arm_timer_t;

static rpi_arm_timer_t* rpi_timer = (rpi_arm_timer_t*)RPI_ARMTIMER_BASE;

void rpi_timer_load(u32 value)
{
	rpi_timer->load = value;
}


void rpi_timer_ctrl(u32 flags)
{
	rpi_timer->control = flags;
}


void rpi_timer_irqclear()
{
	rpi_timer->irqclear = 1;
}

void init_timer(u32 freq)
{
	rpi_basic_interrupt_enable(RPI_BASIC_IRQ_ARM_TIMER);

	rpi_timer_load(freq);

	rpi_timer_ctrl(RPI_ARMTIMER_CTRL_23BIT |
		       RPI_ARMTIMER_CTRL_ENABLE |
		       RPI_ARMTIMER_CTRL_INT_ENABLE |
		       RPI_ARMTIMER_CTRL_PRESCALE_256);
}
