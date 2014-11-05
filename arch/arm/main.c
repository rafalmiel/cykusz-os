#include <stdint.h>

#include <core/io.h>
#include <core/timer.h>

#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "isr.h"
#include "mmu.h"

extern void kernel_main();

void arm_kernel_main(void)
{
	/* Remove identity mapping of 1st MB of ram */
	remove_pt_mapping(0x00000000);

	init_interrupts();

	gpio_select(GPIO_PIN_ACTLED);
	gpio_select(GPIO_PIN_PWRLED);

	kernel_main();
	usb_on();

	while(1) {}
}
