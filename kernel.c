#if !defined(__cplusplus)
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdint.h>

#include <core/io.h>

#if defined(__cplusplus)
extern "C"
#endif

void kernel_main(/*struct multiboot *multiboot*/)
{
	init_output();

	kprint("\n");
	kprint("Hello world kernel!\n");
	//init_vga();

	/*init_descriptor_tables();

	init_paging(multiboot);

	if (multiboot->mods_count > 0) {
		u32 initrd_loc = *(u32*)(multiboot->mods_addr + 0xC0000000);

		vga_writehexnl(initrd_loc);

		vga_writehexnl(*(u32*)(initrd_loc + 0xC0000000 ));
	}*/

}



