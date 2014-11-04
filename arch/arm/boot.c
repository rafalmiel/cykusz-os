#include <core/common.h>
#include <core/io.h>

extern void arm_kernel_main(void);

u32 *s_page_table = (u32 *const)0xC0004000;

__attribute__((naked)) void init_sys(void)
{
	register u32 pt_addr;
	register u32 control;

	asm volatile("push {r0, r1, r2}");

#define MAP(virt, phys) do {s_page_table[(virt >> 20)] = phys | 0x400 | 2;}\
			while(0)

	/* Identity mapping of the first MB of ram, later removed */
	MAP(0, 0);

	/* Mapped kernel at 0xC0000000 */
	MAP(0xC0000000, 0);

	/* Map peripherals regs to 0xD0000000 */
	MAP(0xD0000000, 0x20000000);
	MAP(0xD0100000, 0x20100000);
	MAP(0xD0200000, 0x20200000);

#undef MAP

	pt_addr = (u32) s_page_table;

	/* Translation table 0 - ARM1176JZF-S manual, 3-57 */
	asm volatile("mcr p15, 0, %[addr], c2, c0, 0" : : [addr] "r" (pt_addr));
	/* Translation table 1 */
	asm volatile("mcr p15, 0, %[addr], c2, c0, 1" : : [addr] "r" (pt_addr));
	/* Use translation table 0 for everything, for now */
	asm volatile("mcr p15, 0, %[n], c2, c0, 2" : : [n] "r" (0));

	/* Set Domain 0 ACL to "Client", enforcing memory permissions
	 * See ARM1176JZF-S manual, 3-64
	 * Every mapped section/page is in domain 0
	 */
	asm volatile("mcr p15, 0, %[r], c3, c0, 0" : : [r] "r" (0x1));

	/* Read control register */
	asm volatile("mrc p15, 0, %[control], c1, c0, 0" : [control] "=r" (control));
	/* Turn on MMU */
	control |= 1;
	/* Enable ARMv6 MMU features (disable sub-page AP) */
	control |= (1<<23);
	/* Write value back to control register */
	asm volatile("mcr p15, 0, %[control], c1, c0, 0" : : [control] "r" (control));

	/* Set the LR (R14) to the address of main(), then pop off r0-r2
	 * before exiting this function (which doesn't store anything else
	 * on the stack). The "mov lr" comes first as it's impossible to
	 * guarantee the compiler wouldn't use one of r0-r2 for %[main]
	 */
	asm volatile("mov lr, %[main]" : : [main] "r" ((unsigned int)&arm_kernel_main) );
	asm volatile("pop {r0, r1, r2}");
	asm volatile("bx lr");
}
