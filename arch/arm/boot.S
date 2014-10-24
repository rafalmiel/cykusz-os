.section .init
.globl _start
.globl arm_kernel_main

b _start

.section .text
_start:
	mov sp, #0x8000
	ldr r0, =arm_kernel_main
	bx r0

loop$:
	b loop$
