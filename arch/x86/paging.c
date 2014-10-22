#include "paging.h"
#include "io.h"
#include "kheap.h"
#include "frame.h"

extern page_table_t BootPageEntry;	// Kernel Page entries declared in boot.S
extern u32 BootPageDirectory;		// Kernel Page Dir declared in boot.S

static page_directory_t kernel_pd;

static heap_t heap;

extern u32 __phys_end;

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

	vga_writestring("Page fault! ( ");

	if (present)
		vga_writestring("present ");

	if (rw)
		vga_writestring("read-only ");

	if (us)
		vga_writestring("user-mode ");

	if (reserved)
		vga_writestring("reserved ");

	vga_writestring(") at ");

	vga_writehex(faulting_address);

	vga_writestring("\n");

	asm volatile("hlt");
}

void init_paging(struct multiboot *multiboot)
{
	register_interrupt_handler(14, page_fault);

	init_frames();

	memset(&kernel_pd, 0, sizeof kernel_pd);

	kernel_pd.tablesPhysical = &BootPageDirectory;

	for (u32 i = 0; i < 256; ++i) {
		kernel_pd.tables[768 + i] = &BootPageEntry + i;
	}

	/* Identity mapping of initrd image */
	if (multiboot->mods_count > 0) {
		paging_identity_map_to(multiboot->mods_addr + 4);
		paging_identity_map_to(*(u32*)(multiboot->mods_addr
					       + 0xC0000000
					       + 4)
				       );
	}

	/**
	 * Allocate physical memory for initial heap size
	 */
	for (u32 i = 0xD0000000; i < 0xD0090000; i += 0x1000)
		frame_alloc(page_get(i));

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
		return 0;
	}
}


void paging_identity_map_to(u32 phys_address)
{
	s_current_end = align_4K(s_current_end);

	for (; s_current_end < phys_address; s_current_end += 0x1000) {
		u32 addr = s_current_end + 0xC0000000;

		page_t *page = page_get(addr);

		frame_alloc_at(s_current_end, page);
	}
}
