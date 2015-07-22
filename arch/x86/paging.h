#ifndef PAGING_H
#define PAGING_H

#include <core/common.h>
#include <asm/paging.h>

#include "multiboot.h"


typedef struct page_table
{
	page_t pages[1024];
} page_table_t;

typedef struct page_directory
{
	page_table_t *tables[1024];	// Array of pointers to pagetables
	u32 *tablesPhysical;		// Page directory with ptrs to entries
} page_directory_t;

void init_paging(struct multiboot *multiboot);

void paging_identity_map(u32 phys_address);
void paging_identity_map_to(u32 phys_address);


#endif // PAGING_H
