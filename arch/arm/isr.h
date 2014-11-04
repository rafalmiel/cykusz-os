#ifndef ISR_H
#define ISR_H

#include <core/common.h>

/* Bits in the Enable_Basic_IRQs register to enable various interrupts.
   See the BCM2835 ARM Peripherals manual, section 7.5 */
#define RPI_BASIC_IRQ_ARM_TIMER		(1 << 0)
#define RPI_BASIC_IRQ_ARM_MAILBOX	(1 << 1)
#define RPI_BASIC_IRQ_ARM_DOORBELL_0	(1 << 2)
#define RPI_BASIC_IRQ_ARM_DOORBELL_1	(1 << 3)
#define RPI_BASIC_IRQ_GPU_0_HALTED	(1 << 4)
#define RPI_BASIC_IRQ_GPU_1_HALTED	(1 << 5)
#define RPI_BASIC_IRQ_ACCESS_ERROR_1	(1 << 6)
#define RPI_BASIC_IRQ_ACCESS_ERROR_0	(1 << 7)

void init_interrupts(void);

void rpi_basic_interrupt_enable(u32 source);

#endif // ISR_H
