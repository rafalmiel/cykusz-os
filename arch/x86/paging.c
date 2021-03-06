#include <core/io.h>
#include <core/frame.h>
#include <core/kheap.h>

#include "paging.h"
#include "isr.h"

extern page_table_t BootPageEntry;	// Kernel Page entries declared in boot.S
extern u32 BootPageDirectory;		// Kernel Page Dir declared in boot.S

static page_directory_t kernel_pd;

static heap_t heap;

extern u32 __phys_end;

static u32 s_frames[128];

static u32 s_current_end = (u32)&__phys_end;

static void page_fault(registers_t *regs)
{
	u32 faulting_address;
	asm volatile("mov %%cr2, %0" : "=r" (faulting_address));

	int present	= !(regs->err_code & 0x1);
	int rw		= regs->err_code & 0x2;
	int us		= regs->err_code & 0x4;
	int reserved	= regs->err_code & 0x8;
	//int id		= regs->err_code & 0x10;

	kprint("Page fault! ( ");

	if (present)
		kprint("present ");

	if (rw)
		kprint("read-only ");

	if (us)
		kprint("user-mode ");

	if (reserved)
		kprint("reserved ");

	kprint(") at ");

	kprint_hex(faulting_address);

	kprint("\n");

	asm volatile("hlt");
}

void init_paging(struct multiboot *multiboot)
{
	register_interrupt_handler(14, page_fault);

	init_frames(s_frames, 128, s_current_end);

	memset(&kernel_pd, 0, sizeof kernel_pd);

	kernel_pd.tablesPhysical = &BootPageDirectory;

	for (u32 i = 0; i < 256; ++i) {
		kernel_pd.tables[768 + i] = &BootPageEntry + i;
	}

	/* Identity mapping of initrd image */
	if (multiboot->mods_count > 0) {
		kprint("Additional ident mapping ");
		kprint_hexnl(multiboot->mods_addr + 4);

		paging_kern_map_to(multiboot->mods_addr);
		paging_kern_map_to(*(u32*)(phys_to_virt(multiboot->mods_addr)));
	}

	/**
	 * Allocate physical memory for initial heap size
	 */
	for (u32 i = 0xD0000000; i < 0xD0090000; i += 0x1000)
		frame_alloc(page_get(i), i);

	/**
	 * Max kernel heap size is 256MB
	 */
	init_heap(&heap, 0xD0000000, 0xD0090000, 0xE0000000, 0, 0);

	heap_set_current(&heap);
}

page_t *page_get(u32 address)
{
	address /= 0x1000;

	u32 table_idx = address / 1024;

	/**
	 * TODO: allocate table entries on heap in case they are not present
	 */
	if (kernel_pd.tables[table_idx]) {
		return &kernel_pd.tables[table_idx]->pages[address % 1024];
	} else {
		u32 phys;
		page_table_t *pg = (page_table_t*)kmalloc_ap(sizeof (page_table_t), &phys);
		kernel_pd.tables[table_idx] = pg;
		memset(kernel_pd.tables[table_idx], 0, 0x1000);
		kernel_pd.tablesPhysical[table_idx] = phys | 0x7;
		return &pg->pages[address % 1024];
	}
}


void paging_kern_map_to(u32 phys_address)
{
	s_current_end = align_4K(s_current_end);

	for (; s_current_end <= phys_address; s_current_end += 0x1000) {
		u32 addr = phys_to_virt(s_current_end);

		kprint("Adding mapping for: ");
		kprint_hexnl(s_current_end);

		page_t *page = page_get(addr);

		frame_alloc_at(page, s_current_end, addr);
	}
}

void paging_identity_map(u32 phys_address)
{
	page_t *page;
	if ((phys_address & 0xFFF) != 0) {
		phys_address = align_4K(phys_address - 0x1000);
	}

	page = page_get(phys_address);
//	kprint("MAPPING ");
//	kprint_hex(phys_address);
//	kprint(" TO ");
//	kprint_hexnl((phys_address));
	frame_alloc_at(page, phys_address, phys_address);
}
