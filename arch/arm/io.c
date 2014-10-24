#include <core/io.h>

#include "uart.h"

void init_output()
{
	uart_init();
}

void kprint(const char *str)
{
	uart_puts(str);
}
