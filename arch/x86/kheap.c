#include "kheap.h"
#include "paging.h"
#include "frame.h"
#include "io.h"

static heap_t *s_heap = 0;

u32 heap_malloc(heap_t *heap, u32 size, int a, u32 *phys);

static inline u32 align(u32 addr, u32 to)
{
	if (addr & (to - 1)) {
		addr &= ~(to - 1);
		addr += to;
	}

	return addr;
}

static inline u32 align_4K(u32 addr)
{
	return align(addr, 0x1000);
}

static inline u32 align_32B(u32 addr)
{
	return align(addr, 0x20);
}

u32 kmalloc_int(u32 size, int align, u32 *phys)
{
	return heap_malloc(s_heap, size, align, phys);
}

u32 kmalloc_a(u32 size)
{
	return kmalloc_int(size, 1, 0);
}

u32 kmalloc_p(u32 size, u32 *phys)
{
	return kmalloc_int(size, 0, phys);
}

u32 kmalloc_ap(u32 size, u32 *phys)
{
	return kmalloc_int(size, 1, phys);
}

u32 kmalloc(u32 size)
{
	return kmalloc_int(size, 0, 0);
}

static void expand(heap_t *heap, u32 size)
{
	size = align_4K(size);

	if (heap->end_addr + size <= heap->max_addr) {
		u32 dest = heap->end_addr + size;
		while (heap->end_addr < dest) {
			page_t *page = get_page(heap->end_addr);
			alloc_frame(page);

			heap->end_addr += 0x1000;
		}
	}
}

u32 heap_malloc(heap_t *heap, u32 size, int align, u32 *phys)
{
	u32 placement_address = heap->current_addr;

	if (align == 1) {
		placement_address = align_4K(placement_address);
	} else {
		placement_address = align_32B(placement_address);
	}

	if (placement_address + size > heap->end_addr) {
		expand(heap, placement_address + size - heap->end_addr);
	}

	if (phys) {
		page_t *page = get_page(placement_address);
		*phys = (page->frame * 0x1000) + (placement_address & 0xFFF);
	}

	heap->current_addr = placement_address + size;
	return placement_address;
}

void init_heap(heap_t *heap, u32 start, u32 size, u32 maxend)
{
	heap->start_addr = align_4K(start);
	heap->current_addr = heap->start_addr;
	heap->end_addr = heap->start_addr;
	heap->max_addr = align_4K(maxend);

	expand(heap, size);
}


void heap_set_current(heap_t *heap)
{
	s_heap = heap;
}
