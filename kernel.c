#if !defined(__cplusplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>

#include <core/io.h>
#include <core/timer.h>
#include <core/kheap.h>

#if defined(__cplusplus)
extern "C"
#endif

void kernel_main()
{
	u32 phys;
	kprint("Hello kernel!!!\n");

	u32 addr = kmalloc(40);

	kprint("Allocated new mem at addr: ");
	kprint_hexnl(addr);

	u32 addr2 = kmalloc(40);

	kprint("Allocated new mem at addr: ");
	kprint_hexnl(addr2);

	u32 addr3 = kmalloc(40);

	kprint("Allocated new mem at addr: ");
	kprint_hexnl(addr3);

	kfree(addr2);
	kfree(addr3);
	kfree(addr);

	kprint("Free all memory!\n");

	addr3 = kmalloc_ap(40, &phys);

	kprint("Allocated new mem at addr: ");
	kprint_hex(addr3);
	kprint(". Physical addr: ");
	kprint_hexnl(phys);

	u32 addr4 = kmalloc(40);

	kprint("Allocated new mem at addr: ");
	kprint_hexnl(addr4);

	init_timer(0x100);
}
