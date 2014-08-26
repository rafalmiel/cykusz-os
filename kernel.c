#if !defined(__cplusplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error "You are not using a cross compiler, you will most certainly run into trouble"
#endif

#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

#include "arch/x86/io.h"
#include "arch/x86/descriptor_tables.h"
#include "arch/x86/timer.h"
#include "arch/x86/kheap.h"
#include "arch/x86/paging.h"

#if defined(__cplusplus)
extern "C"
#endif

extern u32 BootPageDirectory;
extern u32 BootPageEntry;
extern u32 VirtualBase;
extern u32 __end;

void kernel_main()
{
	vga_initialize();
	init_descriptor_tables();
	initialise_paging();

	for (u32 i = 0; i < 20; ++i) {
		u32 addr = 0;
		u32* i = (u32*)kmalloc_p(4, &addr);
		*i = 33;
		vga_writehex(*i);
		vga_writestring(" @ ");
		vga_writehex(addr);
		vga_writestring(" @ ");
		vga_writehex((u32)i);
		vga_writestring("\n");
	}
}


