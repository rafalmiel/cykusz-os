.section .kernel_table
.balign 4
.global __kernel_table
__kernel_table: .skip 256*64

.section .init

// Store interrupt vector table at the beginning of the init,
// which will be copied to the 0x0 address of ram
_init:

b _start

.global kprint
.global arm_kernel_main
.global init_sys
.global _start

.section .text
_start:
	mov r4, #0xC0000000

	/* ABORT stack at 0x2400 */
	cps #0x17		/* Change to Abort mode */
	add sp, r4, #0x2800

	/* IRQ stack at 0x2800 */
	cps #0x12		/* Change to IRQ mode */
	add sp, r4, #0x2c00

	/* Supervisor stack at 0x2000 */
	cps #0x13		/* Change to supervisor (SVC) mode */
	add sp, r4, #0x2400

	bl init_sys

loop:
	wfe
	b loop
