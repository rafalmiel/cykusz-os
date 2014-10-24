.section .init
.globl _start

_start:
	ldr r0, =0x20200000

	mov r1,#1
	lsl r1,#21
	str r1,[r0,#16]

	mov r1,#1
	lsl r1,#15
	str r1,[r0,#32]


loop$:
	b loop$
