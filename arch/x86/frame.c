#include "frame.h"
#include <core/io.h>

#define INDEX_FROM_BIT(a) ((a) / (8 * 4))
#define OFFSET_FROM_BIT(a) ((a) % (8 * 4))

static const u32 nframes = 128 * 32;
static u32 frames[128];			// 16MB

extern u32 __phys_end;			// End of kernel in physical mem

static void set_frame(u32 frame_addr)
{
	u32 frame = frame_addr / 0x1000;
	u32 idx = INDEX_FROM_BIT(frame);
	u32 off = OFFSET_FROM_BIT(frame);

	frames[idx] |= (0x1 << off);
}

static void clear_frame(u32 frame_addr)
{
	u32 frame = frame_addr / 0x1000;
	u32 idx = INDEX_FROM_BIT(frame);
	u32 off = OFFSET_FROM_BIT(frame);

	frames[idx] &= ~(0x1 << off);
}

static u32 test_frame(u32 frame_addr)
{
	u32 frame = frame_addr / 0x1000;
	u32 idx = INDEX_FROM_BIT(frame);
	u32 off = OFFSET_FROM_BIT(frame);
	return (frames[idx] & (0x1 << off));
}

static u32 first_frame()
{
	u32 i, j;

	for (i = 0; i < INDEX_FROM_BIT(nframes); ++i) {
		if (frames[i] != 0xFFFFFFFF) {
			for (j = 0; j < 32; ++j) {
				u32 toTest = 0x1 << j;
				if (!(frames[i] & toTest)) {
					return i * 4 * 8 + j;
				}
			}
		}
	}

	return -1;
}

void frame_alloc(page_t *page)
{
	if (page->frame != 0) {
		return;
	} else {
		u32 idx = first_frame();

		if (idx == (u32)-1) {
			vga_writestring("no frames!");
			asm volatile("hlt");
		}

		set_frame(idx * 0x1000);
		page->present = 1;
		page->rw = 1;
		page->user = 1;
		page->frame = idx;
	}

}

void frame_free(page_t *page)
{
	u32 frame;

	if (!(frame = page->frame)) {
		return;
	} else {
		clear_frame(frame);
		page->frame = 0x0;
	}
}

void init_frames()
{
	const u32 fin = ((u32)&__phys_end);

	memset(frames, 0, 4 * 128);
	u32 addr = 0;
	u32 cnt = 0;
	while (addr < fin) {
		set_frame(addr);
		addr += 0x1000;
		cnt++;
	}
}


void frame_alloc_at(u32 phys_addr, page_t *page)
{
	if (test_frame(phys_addr) == 0) {
		set_frame(phys_addr);

		page->present = 1;
		page->rw = 1;
		page->user = 1;
		page->frame = phys_addr / 0x1000;
	}
}
