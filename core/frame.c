#include <core/frame.h>
#include <core/io.h>

#define INDEX_FROM_BIT(a) ((a) / (8 * 4))
#define OFFSET_FROM_BIT(a) ((a) % (8 * 4))

static u32 s_nframes;
static u32 *s_frames;		// 16MB
static u32 s_frames_size;

static void set_frame(u32 frame_addr)
{
	u32 frame = frame_addr / 0x1000;
	u32 idx = INDEX_FROM_BIT(frame);
	u32 off = OFFSET_FROM_BIT(frame);

	if (idx < s_frames_size) {
		s_frames[idx] |= (0x1 << off);
	}
}

static void clear_frame(u32 frame_addr)
{
	u32 frame = frame_addr / 0x1000;
	u32 idx = INDEX_FROM_BIT(frame);
	u32 off = OFFSET_FROM_BIT(frame);

	if (idx < s_frames_size) {
		s_frames[idx] &= ~(0x1 << off);
	}
}

static u32 test_frame(u32 frame_addr)
{
	u32 frame = frame_addr / 0x1000;
	u32 idx = INDEX_FROM_BIT(frame);
	u32 off = OFFSET_FROM_BIT(frame);

	if (idx < s_frames_size) {
		return (s_frames[idx] & (0x1 << off));
	}

	return 0;
}

static u32 first_frame()
{
	u32 i, j;

	for (i = 0; i < INDEX_FROM_BIT(s_nframes); ++i) {
		if (s_frames[i] != 0xFFFFFFFF) {
			for (j = 0; j < 32; ++j) {
				u32 toTest = 0x1 << j;
				if (!(s_frames[i] & toTest)) {
					return i * 4 * 8 + j;
				}
			}
		}
	}

	return -1;
}

void frame_alloc(page_t *page, u32 virt)
{
	//kprint("Page base addr: ");
	//kprint_hexnl(page_base_addr(page));
	if (page_base_addr(page) != 0) {
		return;
	} else {
		u32 idx = first_frame();

		if (idx == (u32)-1) {
			kprint("no frames!");
			while(1){}
		}

		set_frame(idx * 0x1000);

		page_init(page, (idx << 12));
		tlb_invalidate(virt);
	}
}

void frame_free(page_t *page, u32 virt)
{
	u32 frame;

	if (!(frame = page_base_addr(page))) {
		return;
	} else {
		clear_frame(frame);

		page_clear(page);
		tlb_invalidate(virt);
	}
}

void init_frames(u32 *frames_bitmap, u32 frames_size, u32 kernel_end)
{
	const u32 fin = kernel_end;

	s_frames = frames_bitmap;
	s_frames_size = frames_size;
	s_nframes = s_frames_size * 32;

	memset(s_frames, 0, 4 * 128);

	u32 addr = 0;
	u32 cnt = 0;
	while (addr < fin) {
		set_frame(addr);
		addr += 0x1000;
		cnt++;
	}
}


void frame_alloc_at(page_t *page, u32 phys_addr, u32 virt)
{
	if (test_frame(phys_addr) == 0) {
		set_frame(phys_addr);

		page_init(page, phys_addr);
		tlb_invalidate(virt);
	}
}

