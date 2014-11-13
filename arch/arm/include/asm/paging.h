#ifndef ASM_PAGING_H
#define ASM_PAGING_H

#include <core/common.h>
#include <core/io.h>

#define tlb_invalidate(virt) \
	do { \
	asm volatile("mcr p15, 0, %[data], c8, c7, 1" : : [data] "r" (virt)); \
	} \
	while (0)

typedef struct page
{
	u32 xn		: 1;	// eXecute Never
	u32 id		: 1;	// 1 for Extended small page (4KB)
	u32 b		: 1;	// Bufferable
	u32 c		: 1;	// Cacheable
	u32 ap		: 2;	// Access permissions
	u32 tex		: 3;	// Type Extension Field
	u32 apx		: 1;	// Access premission extension
	u32 s		: 1;	// Shared
	u32 ng		: 1;	// Not global
	u32 base_addr	: 20;	// Frame address (shift right 12 bits)
} page_t;

#define page_base_addr(page) page->base_addr

#define page_init(page, phys)			\
	do {					\
		page->xn = 1;			\
		page->id = 1;			\
		page->ap = 1;			\
		page->base_addr = (phys >> 12);	\
	} while(0)

#define page_clear(page)			\
	do {					\
		page->xn = 0;			\
		page->id = 0;			\
		page->ap = 0;			\
		page->base_addr = 0;		\
	} while(0)

page_t *page_get(u32 address);

#endif // ASM_PAGING_H
