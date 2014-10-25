#include <stdint.h>

#include <core/io.h>

#include "gpio.h"
#include "uart.h"
#include "timer.h"
#include "isr.h"

extern void kernel_main();
extern void enable_interrupts();

void arm_kernel_main(void)
{
	gpio_select(GPIO_PIN_ACTLED);
	gpio_select(GPIO_PIN_PWRLED);

	enable_interrupts();

	//init_output();

	kernel_main();
}
