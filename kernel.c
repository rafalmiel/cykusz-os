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

#if defined(__cplusplus)
extern "C"
#endif
void kernel_main()
{
	vga_initialize();
	
	vga_setcolor(vga_makecolor(COLOR_WHITE, COLOR_RED));

	vga_writestring("Hello World!!");
}
