#ifndef MMU_H
#define MMU_H

#include <core/common.h>
#include <core/io.h>
#include <asm/paging.h>

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
	/* Temporary hack */
	return virt - 0xC0000000;
	/*u32 val = s_page_table[virt >> 20];

	return addr_high(phys) + addr_offset(virt);*/
}

static inline void remove_pt_mapping(u32 addr)
{
	s_page_table[addr >> 20] = 0;
	/* Flush it out of the TLB */
	tlb_invalidate(addr);
}

static inline void add_pt_mapping_4k(u32 virt, u32 phys)
{
	kprint("page table entry: ");
	kprint_hexnl(s_page_table[virt >> 20]);
	if (s_page_table[virt >> 20] & 1) {
		page_t *addr = (page_t *)((s_page_table[virt >> 20] & (u32)~(0b1111111111)) + 0xC0000000);

		kprint_hexnl((s_page_table[virt >> 20] & (u32)~(0b1111111111)) + 0xC0000000);

		kprint_hexnl(addr[(virt >> 12) % 0x1000].base_addr);

		page_t *page = &addr[(virt >> 12) % 0x1000];

		kprint("pe idx: ");
		kprint_hexnl((virt >> 12) % 0x1000);

		//*page = (phys << 12) | 0x0010 | 3;
		//*page = phys | 0x0010 | 3;

		page->xn = 1;
		page->id = 1;
		page->ap = 1;
		page->base_addr = (phys >> 12);

		kprint("Mapping added, base addr: ");
		kprint_hexnl(page->base_addr);
	} else {
		kprint("Mapping add failed\n");
	}

	kprint("Flush addr: ");
	kprint_hexnl(virt);

	tlb_invalidate(virt);
}

static inline void add_pt_mapping(u32 virt, u32 phys)
{
	u32 v = addr_high(virt);
	u32 p = addr_high(phys);

	s_page_table[v >> 20] = p | 0x400 | 2;

	tlb_invalidate(v);
}

#endif // MMU_H
