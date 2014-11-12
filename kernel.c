#if !defined(__cplusplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>

#include <core/io.h>
#include <core/timer.h>

//#include <arch/x86/isr.h>
//#include <arch/x86/paging.h>
//#include <arch/x86/kheap.h>

#if defined(__cplusplus)
extern "C"
#endif

void kernel_main(/*struct multiboot *multiboot*/)
{
//	init_paging(multiboot);
	init_timer(0x100);
	init_output();

//	kprint_hexnl(kmalloc(31));
//	kprint_hexnl(kmalloc(31));
//	kprint_hexnl(kmalloc(31));

	kprint("Hello!\n");
}



