#include <core/common.h>
#include <core/io.h>

#include "framebuffer.h"
#include "barrier.h"
#include "mailbox.h"
#include "uart.h"

typedef struct {
	u32 width, height, virt_width, virt_height;
	u32 pitch;
	u32 depth;
	u32 offset_x, offset_y;
	u32 fb_address;
	u32 fb_size;
} fb_set_cmd_t;

void framebuffer_init(void)
{
	volatile fb_set_cmd_t set __attribute__((aligned (16)));

	set.width = 1920;
	set.height = 1080;
	set.virt_width = set.width;
	set.virt_height = set.height;
	set.pitch = 0;
	set.depth = 24;
	set.offset_x = 0;
	set.offset_y = 0;
	set.fb_address = 0;
	set.fb_size = 0;

	mailbox_write(1, (u32)&set + 0x40000000);
	mailbox_read(1);

	u8 bytes_per_pix = set.depth / 8;
	u32 total_pixels = set.fb_size / bytes_per_pix;

	for (u32 i = 0; i < total_pixels; ++i) {
		u8 *addr = (u8*)set.fb_address;

		addr += (i * bytes_per_pix);

		addr[0] = 255;
		addr[1] = 0;
		addr[2] = 255;
	}
}
