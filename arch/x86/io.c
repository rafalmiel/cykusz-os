#include <core/io.h>
#include "common.h"
#include "vga.h"

static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;

static size_t terminal_row;
static size_t terminal_column;
static u8 terminal_color;
static u16 *terminal_buffer;

static void vga_movecursor(void)
{
	u16 position = VGA_WIDTH * terminal_row + terminal_column;
	// cursor LOW port to vga INDEX register
	outb(0x3D4, 0x0F);
	outb(0x3D5, (u8)(position & 0xFF));
	// cursor HIGH port to vga INDEX register
	outb(0x3D4, 0x0E);
	outb(0x3D5, (u8)((position >> 8) & 0xFF));
}

static u16 vga_makeentry(char c, uint8_t color)
{
	u16 c16 = c;
	u16 color16 = color;
	return c16 | color16 << 8;
}

static void vga_putentryat(char c, u8 color, size_t x, size_t y)
{
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_makeentry(c, color);
}

static void vga_scroll(void)
{

	u16 blank = vga_makeentry(' ', terminal_color);

	if (terminal_row > 24) {
		u16 i;
		for (i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; ++i) {
			terminal_buffer[i] = terminal_buffer[i + VGA_WIDTH];
		}

		for (i = (VGA_HEIGHT - 1) * VGA_WIDTH;
		     i < VGA_HEIGHT * VGA_WIDTH;
		     ++i) {
			terminal_buffer[i] = blank;
		}

		terminal_row = VGA_HEIGHT - 1;
	}
}

void kprint_char(char c)
{
	if (c == 0x08 && terminal_column) { //handle backspace
		terminal_column--;
		vga_putentryat(' ',
			       vga_makecolor(COLOR_WHITE, COLOR_BLACK),
			       terminal_column, terminal_row);
	} else if (c == '\n') {
		terminal_column = 0;
		terminal_row++;
	} else if (c == '\r') {
		terminal_column = 0;
	} else if (c >= ' ') {
		vga_putentryat(c, terminal_color, terminal_column,
			       terminal_row);
		terminal_column++;
	}


	if (terminal_column == VGA_WIDTH)
	{
		terminal_column = 0;
		terminal_row++;
	}

	vga_scroll();
}

u8 vga_makecolor(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

void init_vga(void)
{
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_makecolor(COLOR_WHITE, COLOR_BLACK);
	terminal_buffer = (u16*) 0xC00B8000;
	vga_clear();
}

void vga_clear(void)
{
	u16 blank = vga_makeentry(' ', terminal_color);
	for (size_t y = 0; y < VGA_HEIGHT; ++y)
	{
		for (size_t x = 0; x < VGA_WIDTH; ++x)
		{
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = blank;
		}
	}

	terminal_row = 0;
	terminal_column = 0;
	vga_movecursor();
}

void vga_setcolor(u8 color)
{
	terminal_color = color;
}

void vga_writestring(const char *data)
{
	const size_t datalen = strlen(data);
	for (size_t i = 0; i < datalen; ++i)
		kprint_char(data[i]);

	vga_movecursor();
}

void init_output()
{
	init_vga();
}

void kprint(const char *str)
{
	vga_writestring(str);
}
