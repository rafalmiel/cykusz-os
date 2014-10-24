.globl led_on
.globl led_off

led_on:
	push {r0, r1, lr}
	ldr r0, =0x20200000

	mov r1,#1
	lsl r1,#21
	str r1,[r0,#16]

	mov r1,#1
	lsl r1,#15
	str r1,[r0,#32]

	pop {r0, r1, lr}
	bx lr

led_off:
	push {r0, r1, lr}
	ldr r0, =0x20200000

	mov r1,#1
	lsl r1,#21
	str r1,[r0,#16]

	mov r1,#1
	lsl r1,#15
	str r1,[r0,#44]

	pop {r0, r1, lr}
	bx lr
