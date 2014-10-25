#include <stdint.h>

#include "gpio.h"
#include "uart.h"

static void sleep()
{
	for (int i = 0; i < 3000000; ++i)
		asm volatile("mov r0, r0");
	for (int i = 0; i < 3000000; ++i)
		asm volatile("mov r0, r0");
	for (int i = 0; i < 3000000; ++i)
		asm volatile("mov r0, r0");
	for (int i = 0; i < 3000000; ++i)
		asm volatile("mov r0, r0");
}

extern void kernel_main();

void arm_kernel_main(void)
{
	kernel_main();

	gpio_actled_select();
	gpio_pwrled_select();

	for (;;) {
		gpio_actled_on();
		gpio_pwrled_off();
		sleep();

		gpio_actled_off();
		gpio_pwrled_on();
		sleep();
	}
}
