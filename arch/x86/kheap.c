#include "kheap.h"
#include "paging.h"
#include "frame.h"
#include "io.h"

typedef struct
{
	u32 magic;
	u8 is_hole;
	u32 size;
} header_t;

typedef struct
{
	u32 magic;
	header_t *header;
} footer_t;

static heap_t *s_heap = 0;

void *alloc(u32 size, u8 page_align, heap_t *heap);

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
	void *addr = alloc(size, (u8)align, s_heap);
	if (phys != 0) {
		page_t *page = get_page((u32)addr);
		*phys = page->frame * 0x1000 + ((u32)addr & 0xFFF);
	}
	return (u32)addr;
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

void kfree(u32 addr)
{
	free((void*)addr, s_heap);
}

void heap_set_current(heap_t *heap)
{
	s_heap = heap;
}

static s32 find_smallest_hole(u32 size, u8 page_align, heap_t *heap)
{
	u32 iterator = 0;
	while (iterator < heap->index.size) {
		header_t *header =
				(header_t*)lookup_ordered_array(iterator,
								&heap->index);

		if (page_align > 0) {
			u32 location = (u32) header;
			s32 offset = 0;

			if (((location + sizeof(header_t)) & 0xFFFFF000) != 0) {
				offset = 0x1000 - ((location
						   + sizeof(header_t))
						      % 0x1000);
			}
			s32 hole_size = (s32)header->size - offset;

			if (hole_size > (s32)size)
				break;
		} else if (header->size >= size)
			break;

		++iterator;
	}

	if (iterator == heap->index.size)
		return -1;
	else
		return iterator;
}

static s8 header_t_less_than(void* a, void *b)
{
	return ((header_t*)a)->size < ((header_t*)b)->size ? 1 : 0;
}

static void print_header(void *h)
{
	header_t *header = h;
	vga_writestring("Addr: ");
	vga_writehex((u32)header);
	vga_writestring(" is hole: ");
	vga_writeint(header->is_hole);
	vga_writestring(" size: ");
	vga_writehex(header->size);
	vga_writestring(" | footer->header: ");
	vga_writehexnl((u32)((footer_t*)((u32)header
					 + header->size
					 - sizeof(footer_t)))->header);
}

static void print_heap(heap_t *heap)
{
	header_t *header = (header_t*)heap->start_address;

	while ((u32)header < heap->end_address
		&& header->magic == HEAP_MAGIC) {
		print_header(header);

		header = (header_t*)((u32)header + header->size);
	}

	vga_writestring("=================\n");
}

static void expand(u32 new_size, heap_t *heap)
{
	//TODO: check we don't expand beyond max address
	new_size = align_4K(new_size);

	u32 old_size = heap->end_address - heap->start_address;
	u32 i = old_size;

	while (i < new_size) {
		alloc_frame(get_page(heap->start_address + i));
		i += 0x1000;
	}

	heap->end_address = heap->start_address + new_size;
}

static u32 contract(u32 new_size, heap_t *heap)
{
	new_size = align_4K(new_size);

	if (new_size < HEAP_MIN_SIZE)
		new_size = HEAP_MIN_SIZE;

	u32 old_size = heap->end_address - heap->start_address;
	u32 i = old_size - 0x1000;

	while (new_size < i) {
		free_frame(get_page(heap->start_address + i));
		i -= 0x1000;
	}

	heap->end_address = heap->start_address + new_size;
	return new_size;
}

void init_heap(heap_t *heap, u32 start, u32 end, u32 max, u8 supervisor,
	       u8 readonly)
{
	heap->index = place_ordered_array((void*)start, HEAP_INDEX_SIZE,
					  &header_t_less_than);

	start += sizeof(type_t) * HEAP_INDEX_SIZE;

	start = align_4K(start);

	heap->start_address = start;
	heap->end_address = end;
	heap->max_address = max;
	heap->supervisor = supervisor;
	heap->readonly = readonly;

	header_t *hole = (header_t*) start;
	hole->size = end - start;
	hole->magic = HEAP_MAGIC;
	hole->is_hole = 1;

	insert_ordered_array((void*)hole, &heap->index);
}

static header_t *prepare_header(u32 addr, u32 size, u8 is_hole)
{
	header_t *header = (header_t*)addr;

	header->magic = HEAP_MAGIC;
	header->is_hole = is_hole;
	header->size = size;

	return header;
}

static footer_t *prepare_footer(header_t *header)
{
	footer_t *footer = (footer_t*)((u32)header
				       + header->size
				       - sizeof(footer_t));
	footer->magic = HEAP_MAGIC;
	footer->header = header;

	return footer;
}

static header_t *prepare_heap_frame(u32 addr, u32 size, u8 is_hole)
{
	header_t *header = prepare_header(addr, size, is_hole);
	prepare_footer(header);

	return header;
}

static s32 find_last_hole_idx(heap_t *heap)
{
	s32 iterator = 0;
	s32 idx = -1;
	u32 value = 0x0;
	while ((u32)iterator < heap->index.size) {
		u32 tmp = (u32)lookup_ordered_array(iterator,
						    &heap->index);

		if (tmp > value) {
			value = tmp;
			idx = iterator;
		}

		++iterator;
	}

	return idx;
}

static void resize_block_by(header_t *header, s32 resize_by)
{
	header->size += resize_by;
	prepare_footer(header);
}

static footer_t *get_footer_by_header(header_t *header)
{
	return (footer_t*)((u32)header + header->size - sizeof(footer_t));
}

static void *expand_and_alloc(heap_t *heap, u32 new_size, u8 page_align)
{
	u32 old_length = heap->end_address - heap->start_address;
	u32 old_end_address = heap->end_address;

	expand(old_length + new_size, heap);
	u32 new_length = heap->end_address - heap->start_address;

	s32 idx = find_last_hole_idx(heap);

	if (idx == -1) {
		header_t *header = prepare_heap_frame(old_end_address,
						      new_length
						      - old_length,
						      1);

		insert_ordered_array((void*)header, &heap->index);
	} else {
		header_t *header = lookup_ordered_array(idx,
							&heap->index);
		resize_block_by(header, new_length - old_length);
	}

	return alloc(new_size - sizeof(header_t) - sizeof(footer_t),
		     page_align,
		     heap);
}

void *alloc(u32 size, u8 page_align, heap_t *heap)
{
	u32 new_size = size + sizeof(header_t) + sizeof(footer_t);

	s32 iterator = find_smallest_hole(new_size, page_align, heap);

	/* We didn't find hole large enough, expand the heap and try allocating
	 * again */
	if (iterator == -1) {
		/* Expands the heap and calls alloc recursively */
		return expand_and_alloc(heap, new_size, page_align);
	}

	header_t *orig_hole_header =
			(header_t*)lookup_ordered_array(iterator,
							&heap->index);
	u32 orig_hole_pos = (u32)orig_hole_header;
	u32 orig_hole_size = orig_hole_header->size;

	if (orig_hole_size - new_size < sizeof(header_t) + sizeof(footer_t)) {
		size += orig_hole_size - new_size;
		new_size = orig_hole_size;
	}

	/* Allocate at the page boundary */
	if (page_align && orig_hole_pos & 0xFFFFF000) {
		u32 new_location = orig_hole_pos + 0x1000
				- (orig_hole_pos & 0xFFF)
				- sizeof(header_t);

		u32 size = 0x1000 - (orig_hole_pos & 0xFFF) - sizeof(header_t);

		header_t *hole_header = prepare_heap_frame(orig_hole_pos,
							   size, 1);

		orig_hole_pos = new_location;
		orig_hole_size = orig_hole_size - hole_header->size;
	} else {
		remove_ordered_array(iterator, &heap->index);
	}

	/* Create the block for the new allocation */
	header_t *block_header = prepare_heap_frame(orig_hole_pos, new_size, 0);

	/* Create new hole if we get enough space after allocated block */
	if (orig_hole_size - new_size > 0) {
		header_t *hole_header = prepare_header(orig_hole_pos
						       + sizeof(header_t)
						       + size
						       + sizeof(footer_t),
						       orig_hole_size - new_size,
						       1);
		if ((u32)hole_header + hole_header->size < heap->end_address) {
			prepare_footer(hole_header);
		}

		insert_ordered_array((void*)hole_header, &heap->index);
	}

	return (void*)((u32)block_header + sizeof(header_t));
}


void free(void *p, heap_t *heap)
{
	if (p == 0)
		return;

	header_t *header = (header_t*)((u32)p - sizeof(header_t));
	footer_t *footer = get_footer_by_header(header);

	header->is_hole = 1;

	char to_add = 1;

	/* Merge block on the left, if it is also the hole */
	footer_t *test_footer = (footer_t*)((u32)header - sizeof(footer_t));
	if (test_footer->magic == HEAP_MAGIC
	    && test_footer->header->is_hole == 1) {
		u32 cache_size = header->size;
		header = test_footer->header;

		resize_block_by(header, cache_size);

		to_add = 0;
	}

	/* Merge block on the right, if it is also the hole */
	header_t *test_header = (header_t*)((u32)footer + sizeof(footer_t));
	if (test_header->magic == HEAP_MAGIC && test_header->is_hole) {
		resize_block_by(header, test_header->size);

		footer = get_footer_by_header(header);

		remove_ordered_array_by_val(test_header, &heap->index);
	}

	/* Try contracting the heap if the hole is at the end of it */
	if ((u32)footer + sizeof(footer_t) == heap->end_address) {
		u32 old_length = heap->end_address - heap->start_address;
		u32 new_length = contract((u32)header - heap->start_address,
					  heap);

		if (header->size - (old_length - new_length) > 0) {
			resize_block_by(header, -(old_length - new_length));
		} else {
			remove_ordered_array_by_val(test_header, &heap->index);
		}
	}

	/* Add hole to the index if necessary */
	if (to_add == 1)
		insert_ordered_array((void*)header, &heap->index);
}


void debug_heap(heap_t *heap)
{
	print_heap(heap);
}
