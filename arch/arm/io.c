#include <core/io.h>

#include "uart.h"
#include "framebuffer.h"

static int is_fb_init = 0;

void init_output()
{
	framebuffer_init();
	is_fb_init = 1;
}

void kprint_char(char c)
{
	uart_putc(c);
	if (is_fb_init) {
		framebuffer_draw_char(c);
	}
}

void kprint(const char *str)
{
	uart_puts(str);
	if (is_fb_init) {
		framebuffer_draw_string(str);
	}
}

void framebuffer_disable()
{
	is_fb_init = 0;
}
