#include "paging.h"
#include "io.h"
#include "kheap.h"
#include "frame.h"

extern u32 placement_address;		// Defined in kheap.c
extern page_table_t BootPageEntry;
extern u32 BootPageDirectory;

static page_directory_t kernel_page_directory;
static page_directory_t *current_directory = &kernel_page_directory;

static heap_t heap;

void initialise_paging()
{
	initialise_frames();

	memset(&kernel_page_directory, 0, sizeof kernel_page_directory);

	kernel_page_directory.tablesPhysical = &BootPageDirectory;

	for (u32 i = 0; i < 256; ++i) {
		kernel_page_directory.tables[768 + i] = &BootPageEntry + i;
	}

	initialise_heap(&heap, 0xD0000000, 1024 * 4, 0xE0000000);

	register_interrupt_handler(14, page_fault);
}

page_t *get_page(u32 address)
{
	address /= 0x1000;

	u32 table_idx = address / 1024;

	if (kernel_page_directory.tables[table_idx]) {
		return &kernel_page_directory.tables[table_idx]->pages[address % 1024];
	} else {
		return 0;
	}
}

void page_fault(registers_t *regs)
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
