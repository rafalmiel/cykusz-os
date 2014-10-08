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
#include "arch/x86/multiboot.h"

#if defined(__cplusplus)
extern "C"
#endif

void kernel_main(struct multiboot *multiboot)
{
	init_vga();
	init_descriptor_tables();
	init_paging();

	vga_writestring("Multiboot addr: ");
	vga_writehexnl((u32)multiboot);

	u32 a = kmalloc_a(1000000);
	u32 b = kmalloc(8);

	vga_writehexnl(a);
	vga_writehexnl(b);

	kfree(a);
	kfree(b);

	u32 c = kmalloc(12);

	vga_writehexnl(c);

	kfree(c);

	debug_heap();
}



