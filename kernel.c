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

#if defined(__cplusplus)
extern "C"
#endif

void kernel_main()
{
	init_descriptor_tables();

	vga_initialize();
	
	vga_setcolor(vga_makecolor(COLOR_LIGHT_GREEN, COLOR_BLACK));

	vga_clear();

	vga_writestring("System loaded.\n");
	vga_writestring("Interrupts working.\n");

	u32 addr = kmalloc_a(128);
	u32 addr2 = kmalloc_a(128);
	u32 addr3 = kmalloc_a(128);

	vga_writestring("Allocated mem at addr: ");
	vga_writehex(addr);
	vga_writestring(" ");
	vga_writehex(addr2);
	vga_writestring(" ");
	vga_writehex(addr3);
	vga_writestring("\n");

	asm volatile ("int $23");

	//init_timer(50);
}
