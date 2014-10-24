.section .init
.global _start
.global arm_kernel_main

start:

b main


.section .text

main:
	mov sp, #0x8000
	ldr r0, =arm_kernel_main
	blx r0
