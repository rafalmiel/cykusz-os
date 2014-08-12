#include "io.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static size_t terminal_row;
static size_t terminal_column;
static u8 terminal_color;
static u16 *terminal_buffer;


static u16
vga_makeentry(char c, uint8_t color)
{
	u16 c16 = c;
	u16 color16 = color;
	return c16 | color16 << 8;
}

static void
vga_putentryat(char c, u8 color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_makeentry(c, color);
}

static void
vga_putchar(char c)
{
	vga_putentryat(c, terminal_color, terminal_column, terminal_row);
	if ( ++terminal_column == VGA_WIDTH )
	{
		terminal_column = 0;
		if ( ++terminal_row == VGA_HEIGHT )
		{
			terminal_row = 0;
		}
	}
}

u8
vga_makecolor(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

void
vga_initialize()
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_makecolor(COLOR_WHITE, COLOR_BLACK);
	terminal_buffer = (u16*) 0xB8000;
	for (size_t y = 0; y < VGA_HEIGHT; ++y)
	{
		for (size_t x = 0; x < VGA_WIDTH; ++x)
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_makeentry(' ', terminal_color);
		}
	}
}

void
vga_setcolor(u8 color)
{
	terminal_color = color;
}

void
vga_writestring(const char *data)
{
	const size_t datalen = strlen(data);
	for (size_t i = 0; i < datalen; ++i)
		vga_putchar(data[i]);
}
