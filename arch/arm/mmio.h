#ifndef MMIO_H
#define MMIO_H

#include <core/common.h>

static inline void mmio_write(u32 reg, u32 data) {
    u32 *ptr = (uint32_t*)reg;
    asm volatile("str %[data], [%[reg]]"
	     : : [reg]"r"(ptr), [data]"r"(data));
}

// read from MMIO register
static inline u32 mmio_read(u32 reg) {
    u32 *ptr = (u32*)reg;
    u32 data;
    asm volatile("ldr %[data], [%[reg]]"
		 : [data]"=r"(data) : [reg]"r"(ptr));
    return data;
}

#endif // MMIO_H
