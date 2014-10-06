#ifndef PAGING_H
#define PAGING_H

#include <core/common.h>
#include "isr.h"

typedef struct page
{
	u32 present	: 1;	// Page present in memory
	u32 rw		: 1;	// Read-only if clear, readwrite if set
	u32 user	: 1;	// Supervisor level only if clear
	u32 aaccessed	: 1;	// Has the page been accessed since last refresh
	u32 dirty	: 1;	// Has the page been written since last refresh
	u32 unused	: 7;	// Amalgamation of unused and reserved bits
	u32 frame	: 20;	// Frame address (shift right 12 bits)
} page_t;

typedef struct page_table
{
	page_t pages[1024];
} page_table_t;

typedef struct page_directory
{
	page_table_t *tables[1024];	// Array of pointers to pagetables
	u32 *tablesPhysical;		// Page directory with ptrs to entries
} page_directory_t;

void init_paging();

page_t *get_page(u32 address);

void dbg_heap();


#endif // PAGING_H
