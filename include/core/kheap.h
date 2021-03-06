#ifndef KHEAP_H
#define KHEAP_H

#define HEAP_MAGIC 0xDEADBEEF
#define HEAP_MIN_SIZE 0x10000
#define HEAP_INDEX_SIZE 0x20000

#include <core/common.h>
#include <core/ordered_array.h>

typedef struct heap
{
	ordarr_t index;
	u32 start_address;
	u32 end_address;
	u32 max_address;
	u8 supervisor;
	u8 readonly;
} heap_t;

u32 kmalloc_int(u32 size, int align, u32 *phys);
u32 kmalloc_a(u32 size);
u32 kmalloc_p(u32 size, u32 *phys);
u32 kmalloc_ap(u32 size, u32 *phys);
u32 kmalloc(u32 size);
void kfree(u32 addr);

void heap_set_current(heap_t *heap);

void init_heap(heap_t *heap, u32 start, u32 end, u32 max,
	       u8 supervisor,
	       u8 readonly);

void debug_heap();

#endif // KHEAP_H
