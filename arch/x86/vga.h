#ifndef VGA_H
#define VGA_H

enum vga_color
{
	COLOR_BLACK = 0,
	COLOR_BLUE = 1,
	COLOR_GREEN = 2,
	COLOR_CYAN = 3,
	COLOR_RED = 4,
	COLOR_MAGENTA = 5,
	COLOR_BROWN = 6,
	COLOR_LIGHT_GREY = 7,
	COLOR_DARK_GREY = 8,
	COLOR_LIGHT_BLUE = 9,
	COLOR_LIGHT_GREEN = 10,
	COLOR_LIGHT_CYAN = 11,
	COLOR_LIGHT_RED = 12,
	COLOR_LIGHT_MAGENTA = 13,
	COLOR_LIGHT_BROWN = 14,
	COLOR_WHITE = 15,
};

void init_vga(void);

void vga_clear(void);

u8 vga_makecolor(enum vga_color fg, enum vga_color bg);

void vga_setcolor(u8 color);

void vga_writestring(const char* data);
void kprint_int(u32 val);
void kprint_intnl(u32 val);
void kprint_hex(u32 val);
void kprint_hexnl(u32 val);

#endif // VGA_H
