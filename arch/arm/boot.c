#include <core/common.h>
#include <core/io.h>

#include <asm/paging.h>

extern void arm_kernel_main(void);

extern u32 __kernel_table;

u32 *s_page_table = (u32 *const)0x00004000;
page_t *s_kernel_table = (page_t *const)((u32)&__kernel_table);

extern u32 __kernel_init_start;
extern u32 __kernel_text_start;
extern u32 __kernel_table_start;
extern u32 __kernel_data_start;
extern u32 __kernel_bss_start;
extern u32 __kernel_bss_end;

__attribute__((naked)) void init_sys(void)
{
	register u32 pt_addr;
	register u32 control;
	register u32 x;
	register u32 addr;
	register u32 init_start = (u32)&__kernel_init_start;
	register u32 table_start = (u32)&__kernel_table_start;
	register u32 data_start = (u32)&__kernel_data_start;
	register u32 bss_start = (u32)&__kernel_bss_start;
	register u32 bss_end = (u32)&__kernel_bss_end;

	asm volatile("push {r0, r1, r2}");

	kprint("test\n");

#define MAP(virt, phys) do {s_page_table[(virt >> 20)] = phys | 0x400 | 2;}\
			while(0)

	/* Identity mapping of the first MB of ram, later removed */
	MAP(0, 0);

	/* Map whole IO address space (0x20000000 - 0x20FFFFFFF) */
	for (x = 0; x < 16; ++x) {
		MAP((0xD0000000 + (x << 20)), (0x20000000 + (x << 20)));
	}

#undef MAP

	/* Zero out kernel table */
	for (x = table_start; x < data_start; ++x) {
		*(u32*)x = 0;
	}

	/* Put pointers to empty kernel table for data region
	 * 0xC0000000 - 0xD0000000 */
	for (x = (0xC0000000 >> 20); x < (0xC8000000 >> 20); ++x) {
		s_page_table[x] = ((u32)s_kernel_table + (x - 3072) * 0x400)
				| 1;
	}

	for (x = 0; x < 256; ++x) {
		addr = (x << 12);

		if (addr < init_start) {
			/* Map first 0x8000 as read-write data*/
			s_kernel_table[x].xn = 1;
			s_kernel_table[x].id = 1;
			s_kernel_table[x].ap = 3;
			s_kernel_table[x].base_addr = x;
		} else if (addr < table_start) {
			/* Map kernel code as executable read only*/
			s_kernel_table[x].xn = 0;
			s_kernel_table[x].id = 1;
			s_kernel_table[x].ap = 0x1;
			s_kernel_table[x].apx = 1;
			s_kernel_table[x].base_addr = x;
		} else if (addr < bss_end) {
			/* Map kernel data as not executable read-write*/
			s_kernel_table[x].id = 1;
			s_kernel_table[x].xn = 1;
			s_kernel_table[x].ap = 0x1;
			s_kernel_table[x].base_addr = x;
		} else {
			s_kernel_table[x].id = 0;
			s_kernel_table[x].xn = 0;
			s_kernel_table[x].ap = 0;
			s_kernel_table[x].base_addr = 0;
		}
	}

	/* Zero out bss region */
	for (x = bss_start; x < bss_end; x+=4) {
		*(u32*)x = 0;
	}

	pt_addr = (u32) s_page_table;

	/* Translation table 0 - ARM1176JZF-S manual, 3-57 */
	asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : :
		     [addr] "r" (pt_addr));

	/* Translation table 1 */
	asm volatile("mcr p15, 0, %[addr], c2, c0, 1" : :
		     [addr] "r" (pt_addr));

	/* Use translation table 0 for everything, for now */
	asm volatile("mcr p15, 0, %[n], c2, c0, 2" : :
		     [n] "r" (0));

	/* Set Domain 0 ACL to "Client", enforcing memory permissions
	 * See ARM1176JZF-S manual, 3-64
	 * Every mapped section/page is in domain 0
	 */
	asm volatile("mcr p15, 0, %[r], c3, c0, 0" : :
		     [r] "r" (0x1));

	/* Read control register */
	asm volatile("mrc p15, 0, %[control], c1, c0, 0" :
		     [control] "=r" (control));

	/* Turn on MMU */
	control |= 1;

	/* Enable ARMv6 MMU features (disable sub-page AP) */
	control |= (1<<23);

	/* Write value back to control register */
	asm volatile("mcr p15, 0, %[control], c1, c0, 0" : :
		     [control] "r" (control));

	/* Set the LR (R14) to the address of main(), then pop off r0-r2
	 * before exiting this function (which doesn't store anything else
	 * on the stack). The "mov lr" comes first as it's impossible to
	 * guarantee the compiler wouldn't use one of r0-r2 for %[main]
	 */
	asm volatile("mov lr, %[main]" : :
		     [main] "r" ((unsigned int)&arm_kernel_main) );
	asm volatile("pop {r0, r1, r2}");
	asm volatile("bx lr");
}
