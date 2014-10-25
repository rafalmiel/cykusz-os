#ifndef ARM_TIMER_H
#define ARM_TIMER_H

#include <core/common.h>

/* 0 : 16-bit counters - 1 : 23-bit counter */
#define RPI_ARMTIMER_CTRL_23BIT		( 1 << 1 )

#define RPI_ARMTIMER_CTRL_PRESCALE_1	( 0 << 2 )
#define RPI_ARMTIMER_CTRL_PRESCALE_16	( 1 << 2 )
#define RPI_ARMTIMER_CTRL_PRESCALE_256	( 2 << 2 )

/* 0 : Timer interrupt disabled - 1 : Timer interrupt enabled */
#define RPI_ARMTIMER_CTRL_INT_ENABLE	( 1 << 5 )
#define RPI_ARMTIMER_CTRL_INT_DISABLE	( 0 << 5 )

/* 0 : Timer disabled - 1 : Timer enabled */
#define RPI_ARMTIMER_CTRL_ENABLE	( 1 << 7 )
#define RPI_ARMTIMER_CTRL_DISABLE	( 0 << 7 )

void rpi_timer_load(u32 value);
void rpi_timer_ctrl(u32 flags);
void rpi_timer_irqclear(void);

#endif // ARM_TIMER_H
