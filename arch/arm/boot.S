.section .init

// Store interrupt vector table at the beginning of the init,
// which will be copied to the 0x0 address of ram
_init:

.global kprint
.global arm_kernel_main
.global _start

.section .text
_start:
	mov sp, #0x4000
	bl prepare_interrupt_vector

	bl arm_kernel_main

loop:
	wfe
	b loop
