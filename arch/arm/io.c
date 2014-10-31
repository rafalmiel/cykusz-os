#include <core/io.h>

#include "uart.h"
#include "framebuffer.h"

void init_output()
{
	framebuffer_init();
	//uart_init();
}

void kprint_char(char c)
{
	framebuffer_draw_char(c);
}

void kprint(const char *str)
{
	uart_puts(str);
	framebuffer_draw_string(str);
}
