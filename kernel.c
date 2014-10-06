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
	init_vga();
	init_descriptor_tables();
	init_paging();

	vga_writestring("malloc 8 a\n");
	u32 a = kmalloc(8);
	vga_writestring("malloc 8 b\n");
	u32 b = kmalloc(8);

	//vga_writehexnl(a);
	//vga_writehexnl(b);

	vga_writestring("free b\n");
	kfree(b);
	vga_writestring("free a\n");
	kfree(a);

	vga_writestring("malloc 12 c\n");
	u32 c = kmalloc(12);

	vga_writestring("free c\n");
	kfree(c);
}



