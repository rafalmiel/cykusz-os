#include <core/common.h>

#include "framebuffer.h"
#include "barrier.h"
#include "mailbox.h"

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
	set.depth = 16;
	set.offset_x = 0;
	set.offset_y = 0;
	set.fb_address = 0;
	set.fb_size = 0;

	mailbox_write(1, (u32)&set + 0x40000000);
	mailbox_read(1);

	memset((void*)set.fb_address, 255, set.fb_size/2);
	memset((void*)(set.fb_address + set.fb_size/2), 128, set.fb_size/2);
}
