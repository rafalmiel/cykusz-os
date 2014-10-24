.globl led_on
.globl led_off

led_on:
	push {lr}
	ldr r0, =0x20200000

	mov r1,#1
	lsl r1,#21
	str r1,[r0,#16]

	mov r1,#1
	lsl r1,#15
	str r1,[r0,#32]

	pop {lr}
	bx lr

led_off:
	push {lr}
	ldr r0, =0x20200000

	mov r1,#1
	lsl r1,#21
	str r1,[r0,#16]

	mov r1,#1
	lsl r1,#15
	str r1,[r0,#44]

	pop {lr}
	bx lr
