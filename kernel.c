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

	u32 a = kmalloc_a(8);
	u32 b = kmalloc_a(8);
	//u32 c = kmalloc_a(8);
//	u32 d = kmalloc(8);
//	u32 e = kmalloc(8);

	dbg_heap();

	kfree(a);
	dbg_heap();
	kfree(b);
	dbg_heap();
	//kfree(c);
	//dbg_heap();
//	kfree(d);
//	kfree(e);
}



