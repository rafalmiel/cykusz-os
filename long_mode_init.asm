global long_mode_start

extern enable_interrupts

section .text
bits 64
long_mode_start:
	
	call enable_interrupts
	
	sti
	
	hlt