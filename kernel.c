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

extern u32 __end;

void kernel_main(struct multiboot *multiboot)
{
	init_vga();

	init_descriptor_tables();

	init_paging(multiboot);

	if (multiboot->mods_count > 0) {
		u32 initrd_loc = *(u32*)(multiboot->mods_addr + 0xC0000000);

		vga_writehexnl(initrd_loc);

		vga_writehexnl(*(u32*)(initrd_loc + 0xC0000000 ));
	}

}



