#ifndef KHEAP_H
#define KHEAP_H

#include <core/common.h>

typedef struct heap
{
	u32 current_addr;
	u32 start_addr;
	u32 end_addr;
	u32 max_addr;
} heap_t;

u32 kmalloc_int(u32 size, int align, u32 *phys);
u32 kmalloc_a(u32 size);
u32 kmalloc_p(u32 size, u32 *phys);
u32 kmalloc_ap(u32 size, u32 *phys);
u32 kmalloc(u32 size);

void initialise_heap(heap_t *heap, u32 start, u32 size, u32 maxend);

u32 *getmem(heap_t *heap);

#endif // KHEAP_H
