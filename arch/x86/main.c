#include <core/io.h>
#include <asm/paging.h>

#include "descriptor_tables.h"
#include "paging.h"
#include "multiboot.h"
#include "task.h"
#include "acpi.h"

extern u32 __phys_end;

extern void kernel_main();

void x86_kernel_main(struct multiboot *multiboot)
{
	init_descriptor_tables();
	init_output();
	init_paging(multiboot);

	kprint("Stack pointer: ");
	kprint_hexnl(read_esp());
	kprint("Mod count: ");
	kprint_hexnl(multiboot->mods_count);
	kprint("Kernel phys end : ");
	kprint_hexnl((u32)&__phys_end);
	kprint("Mod addr start : ");
	kprint_hexnl(*(u32*)(phys_to_virt(multiboot->mods_addr)));
	kprint("Mod addr end : ");
	kprint_hexnl(*(u32*)(phys_to_virt(multiboot->mods_addr) + 4));

	init_acpi();
	init_tasking();


	kernel_main();

	if (multiboot->mods_count == 1) {
		kprint("Executing module..\n");

		typedef void (*module_fun)(void);

		u32 modaddr = phys_to_virt(multiboot->mods_addr);
		modaddr = phys_to_virt(*(u32*)modaddr);

		kprint("Modadr is ");
		kprint_hexnl(modaddr);
		kprint_hexnl(*(u32*)modaddr);

		module_fun module = (module_fun)modaddr;

		module();

	}
}
