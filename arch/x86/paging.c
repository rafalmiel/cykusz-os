#include "paging.h"
#include "io.h"
#include "kheap.h"
#include "frame.h"

extern u32 placement_address;		// Defined in kheap.c
extern page_table_t BootPageEntry;
extern u32 BootPageDirectory;

static page_directory_t kernel_page_directory;
static page_directory_t *current_directory = &kernel_page_directory;

void initialise_paging()
{
	initialise_frames();

	vga_writehex((u32)placement_address);

	memset(&kernel_page_directory, 0, sizeof kernel_page_directory);

	kernel_page_directory.tablesPhysical = &BootPageDirectory;
	kernel_page_directory.tables[768] = &BootPageEntry;
	kernel_page_directory.tables[769] = &(BootPageEntry + 1024);

//	u32 mem_end_page = 0x10000000;

//	nframes = mem_end_page / 0x1000;
//	frames = (u32*)kmalloc(INDEX_FROM_BIT(nframes));
//	memset(frames, 0, INDEX_FROM_BIT(nframes));

//	kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
//	current_directory = kernel_directory;

//	kernel_directory->tablesPhysical = BootPageDirectory;

//	/*u32 i = 0;
//	while (i < placement_address) {
//		alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
//		i += 0x1000;
//	}*/

	register_interrupt_handler(14, page_fault);
}

page_t *get_page(u32 address, int make, page_directory_t *dir)
{
	address /= 0x1000;

	u32 table_idx = address / 1024;

	if (dir->tables[table_idx]) {
		return &dir->tables[table_idx]->pages[address % 1024];
	} else if (make) {
		vga_writestring("make new page table!\n");
		u32 tmp;
		dir->tables[table_idx] =
				(page_table_t*)supermalloc(sizeof(page_table_t),
							  &tmp);
		memset(dir->tables[table_idx], 0, 0x1000);
		dir->tablesPhysical[table_idx] = tmp | 0x7; //PRESENT, RW, US
		return &dir->tables[table_idx]->pages[address % 1024];
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


u32 supermalloc(u32 size)
{
	u32 phys;
	u32 addr = kmalloc_ap(size, &phys);
	alloc_frame(get_page(addr, 1, &kernel_page_directory));
	return addr;
}
