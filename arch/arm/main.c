#include <stdint.h>

#include "uart.h"

extern void led_on();
extern void led_off();

void sleep()
{
	for (int i = 0; i < 1000000; ++i){}
}

// kernel main function, it all begins here
void arm_kernel_main(void) {
	uart_init();

	uart_puts("\nHello world");

	for (;;) {
		led_on();
		sleep();
		led_off();
		sleep();
	}
}
