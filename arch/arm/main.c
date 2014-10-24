#include "mmio.h"
#include "uart.h"

void arm_kernel_main()
{
	uart_init();

	uart_puts("Hello world\n");
}
