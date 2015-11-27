global long_mode_start

extern enable_interrupts

section .text
bits 64
long_mode_start:
	
	call enable_interrupts

	sti
	int 33
	mov [0xb8100], word 0x4234243

	hlt
