#ifndef MMU_H
#define MMU_H

#include <core/common.h>
#include <core/io.h>

static u32 *s_page_table = (u32*)0xC0004000;

static inline u32 addr_offset(u32 addr)
{
	return addr % 0x00100000;
}

static inline u32 addr_high(u32 addr)
{
	return addr - addr_offset(addr);
}

static inline u32 virt_to_phys(u32 virt)
{
	u32 phys = s_page_table[virt >> 20];

	return addr_high(phys) + addr_offset(virt);
}

static inline void remove_pt_mapping(u32 addr)
{
	s_page_table[addr >> 20] = 0;
	/* Flush it out of the TLB */
	asm volatile("mcr p15, 0, %[data], c8, c7, 1" : : [data] "r" (addr));
}

static inline void add_pt_mapping(u32 virt, u32 phys)
{
	u32 v = addr_high(virt);
	u32 p = addr_high(phys);

	s_page_table[v >> 20] = p | 0x400 | 2;

	asm volatile("mcr p15, 0, %[data], c8, c7, 1" : : [data] "r" (v));
}

#endif // MMU_H
