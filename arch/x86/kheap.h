#ifndef KHEAP_H
#define KHEAP_H

#include <core/common.h>

u32 kmalloc_int(u32 size, int align, u32 *phys);
u32 kmalloc_a(u32 size);
u32 kmalloc_p(u32 size, u32 *phys);
u32 kmalloc_ap(u32 size, u32 *phys);
u32 kmalloc(u32 size);

#endif // KHEAP_H
