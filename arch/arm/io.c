#include <core/io.h>

#include "uart.h"
#include "framebuffer.h"

void init_output()
{
	framebuffer_init();
	//uart_init();
}

void kprint(const char *str)
{
	uart_puts(str);
}
