#include <core/common.h>
#include <core/io.h>

#include "framebuffer.h"
#include "barrier.h"
#include "mailbox.h"
#include "uart.h"
#include "font.h"
#include "gpio.h"
#include "mmu.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define CHAR_WIDTH 6
#define CHAR_HEIGHT 10

typedef struct {
	u32 width, height, virt_width, virt_height;
	u32 pitch;
	u32 depth;
	u32 offset_x, offset_y;
	u32 fb_address;
	u32 fb_size;
} fb_set_cmd_t;

u32 s_pos_x = 0;
u32 s_pos_y = 0;
u32 s_max_x = 0;
u32 s_max_y = 0;

void *s_fb_addr = 0;
u32 s_pitch = 0;

u32 s_fg_color = 0b1111111111111111;
u32 s_bg_color = 0b0000000000000000;

static void new_line()
{
	s_pos_x = 0;
	s_pos_y++;
}

void framebuffer_draw_char(char ch)
{
	if (ch == '\n') {
		new_line();
		return;
	} else if (ch == '\r') {
		s_pos_x = 0;
		return;
	}

	u16 *addr = (u16*)s_fb_addr
			+ (CHAR_HEIGHT * SCREEN_WIDTH) * s_pos_y
			+ s_pos_x * CHAR_WIDTH;

	if (ch >= ' ' && ch <= '~') {
		int c = ch - ' ';

		u8 *tpl = s_font[c];

		for (u8 row = 0; row < CHAR_HEIGHT; ++row) {

			u8 pxcol = (row > 0) ? tpl[row - 1] : 0;

			for (s32 col = CHAR_WIDTH - 1; col >= 0; --col) {
				if (row < CHAR_HEIGHT
				    && pxcol & (1 << col)) {
					*addr = s_fg_color;
				} else {
					*addr = s_bg_color;
				}

				addr++;
			}

			addr += (SCREEN_WIDTH - CHAR_WIDTH);
		}
	}

	if (++s_pos_x > s_max_x) {
		new_line();
	}
}

void framebuffer_draw_string(const char *str)
{
	char c;
	while ((c = ((char)*str++))) {
		framebuffer_draw_char(c);
	}
}

void framebuffer_init(void)
{
	volatile fb_set_cmd_t set __attribute__((aligned (16)));

	set.width = SCREEN_WIDTH;
	set.height = SCREEN_HEIGHT;
	set.virt_width = set.width;
	set.virt_height = set.height;
	set.pitch = 0;
	set.depth = 16;
	set.offset_x = 0;
	set.offset_y = 0;
	set.fb_address = 0;
	set.fb_size = 0;

	mailbox_write(1, virt_to_phys((u32)&set) + 0x40000000);
	mailbox_read(1);

	u32 offset = addr_offset(set.fb_address);

	/* Map framebuffer to 0xE0000000 */
	add_pt_mapping(0xE0000000, addr_high(set.fb_address));

	s_fb_addr = (void*)(0xE0000000 + offset);
	s_pitch = set.pitch;
	s_pos_x = s_pos_y = 0;

	s_max_x = set.width / CHAR_WIDTH - 1;
	s_max_y = set.height / CHAR_HEIGHT - 1;

	u8 bytes_per_pix = set.depth / 8;
	u32 total_pixels = set.fb_size / bytes_per_pix;

	for (u32 i = 0; i < total_pixels; ++i) {
		u8 *addr = (u8*)s_fb_addr;

		addr += (i * bytes_per_pix);

		addr[0] = 0;
		addr[1] = 0;
	}
}
