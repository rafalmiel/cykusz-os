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

void kernel_main()
{
	init_descriptor_tables();

	vga_initialize();

	initialise_paging();
	
	vga_setcolor(vga_makecolor(COLOR_LIGHT_GREEN, COLOR_BLACK));

	vga_clear();

	vga_writestring("System loaded.\n");

	u32 *ptr = (u32*)0xA0000000;
	u32 do_page_fault = *ptr;		// Test page fault

	*ptr = 42;
}
