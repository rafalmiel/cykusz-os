#include <core/io.h>

#include "descriptor_tables.h"
#include "paging.h"
#include "multiboot.h"

extern void kernel_main();

void x86_kernel_main(struct multiboot *multiboot)
{
	init_descriptor_tables();
	init_output();
	init_paging(multiboot);

	kernel_main();
}
