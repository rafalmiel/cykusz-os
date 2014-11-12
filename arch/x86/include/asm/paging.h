#ifndef ASM_PAGING_H
#define ASM_PAGING_H

#include <core/common.h>

typedef struct page
{
	u32 present	: 1;	// Page present in memory
	u32 rw		: 1;	// Read-only if clear, readwrite if set
	u32 user	: 1;	// Supervisor level only if clear
	u32 accessed	: 1;	// Has the page been accessed since last refresh
	u32 dirty	: 1;	// Has the page been written since last refresh
	u32 unused	: 7;	// Amalgamation of unused and reserved bits
	u32 frame	: 20;	// Frame address (shift right 12 bits)
} page_t;

#define page_base_addr(page) page->frame

#define page_init(page, phys)			\
	do {					\
		page->present = 1;		\
		page->rw = 1;			\
		page->user = 1;			\
		page->frame = ((phys) >> 12);	\
	} while(0)

#define page_clear(page)			\
	do {					\
		page->frame = 0;		\
	} while(0)

#endif // ASM_PAGING_H
