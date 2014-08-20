#include "kheap.h"

extern u32 __end;		//defined in linker script
static u32 placement_address = (u32)&__end;

u32 kmalloc_int(u32 size, int align, u32 *phys)
{
	if (align == 1 && (placement_address & 0xFFFFF000)) {
		// align the placement address
		placement_address &= 0xFFFFF000;
		placement_address += 0x1000;
	}

	if (phys) {
		*phys = placement_address;
	}

	u32 tmp = placement_address;
	placement_address += size;
	return tmp;
}

u32 kmalloc_a(u32 size)
{
	return kmalloc_int(size, 1, 0);
}

u32 kmalloc_p(u32 size, u32 *phys)
{
	return kmalloc_int(size, 0, phys);
}

u32 kmalloc_ap(u32 size, u32 *phys)
{
	return kmalloc_int(size, 1, phys);
}

u32 kmalloc(u32 size)
{
	return kmalloc_int(size, 0, 0);
}
