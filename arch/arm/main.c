#include <stdint.h>

#include "uart.h"

extern void led_on();
extern void led_off();

static void sleep()
{
	for (int i = 0; i < 1000000; ++i){}
}

extern void kernel_main();

void arm_kernel_main(void)
{
	kernel_main();

	for (;;) {
		led_on();
		sleep();
		led_off();
		sleep();
	}
}
