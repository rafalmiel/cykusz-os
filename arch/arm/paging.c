#include <asm/paging.h>
#include <core/frame.h>
#include <core/kheap.h>
#include <core/io.h>

extern u32 __kernel_bss_end;
extern u32 __kernel_table;

static u32 *s_page_table = (u32 *const)0xC0004000;
//static page_t *s_kernel_table = (page_t *const)((u32)&__kernel_table);

static heap_t heap;

page_t *page_get(u32 address)
{
	u32 idx = (address >> 20);
	page_t *page = 0;

	if (s_page_table[idx] & 1) {
		u32 addr = ((s_page_table[idx] & (u32)~(0b1111111111)) + 0xC0000000);

		page = (page_t *)addr;

		page += ((address >> 12) % 0x1000);
	}

	return page;
}

void init_paging()
{
	kprint("Init paging\n");
	u32 bss_end = (u32)&__kernel_bss_end;
	u32 heap_start = align_4K(bss_end) + 0xC0000000;
	init_frames((u32*)0xC0000000, 128, (u32)&__kernel_bss_end);

	/**
	 * Allocate physical memory for initial heap size
	 */
	for (u32 i = heap_start; i < (heap_start + 0x90000); i += 0x1000) {
		//if (i % 10 == 0) kprint_hexnl(i);
		frame_alloc(page_get(i));
		tlb_invalidate(i);
	}

	memset((void*)heap_start, 0, 0x90000);

	/**
	 * Max kernel heap size is 32MB
	 */
	init_heap(&heap, heap_start, (heap_start + 0x90000), 0xC2000000, 0, 0);

	heap_set_current(&heap);
}
