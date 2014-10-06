#include "paging.h"
#include "io.h"
#include "kheap.h"
#include "frame.h"

extern page_table_t BootPageEntry;	// Kernel Page entries declared in boot.S
extern u32 BootPageDirectory;		// Kernel Page Dir declared in boot.S

static page_directory_t kernel_pd;

static heap_t heap;

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

void init_paging()
{
	initialise_frames();

	memset(&kernel_pd, 0, sizeof kernel_pd);

	kernel_pd.tablesPhysical = &BootPageDirectory;

	for (u32 i = 0; i < 256; ++i) {
		kernel_pd.tables[768 + i] = &BootPageEntry + i;
	}

	/**
	 * Allocate physical memory for initial heap size
	 */
	for (u32 i = 0xD0000000; i < 0xD0090000; i += 0x1000)
		alloc_frame(get_page(i));

	/**
	 * Max kernel heap size is 256MB
	 */
	init_heap(&heap, 0xD0000000, 0xD0090000, 0xE0000000, 0, 0);

	heap_set_current(&heap);

	register_interrupt_handler(14, page_fault);
}

page_t *get_page(u32 address)
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


void dbg_heap()
{
	debug_heap(&heap);
}
