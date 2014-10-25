.global _start
.global int_undefined
.global int_software
.global int_prefetch_abort
.global int_data_abort
.global int_interrupt
.global int_fast_interrupt

.section .isr_vector
isr_vector:
ldr pc, addr_start
ldr pc, addr_int_undefined
ldr pc, addr_int_software
ldr pc, addr_int_prefetch_abort
ldr pc, addr_int_data_abort
nop
ldr pc, addr_int_interrupt
ldr pc, addr_int_fast_interrupt

addr_start:			.word	_start
addr_int_undefined:		.word	int_undefined
addr_int_software:		.word	int_software
addr_int_prefetch_abort:	.word	int_prefetch_abort
addr_int_data_abort:		.word	int_data_abort
addr_int_interrupt:		.word	int_interrupt
addr_int_fast_interrupt:	.word	int_fast_interrupt

.equ	CPSR_MODE_USER,		0x10
.equ	CPSR_MODE_FIQ,		0x11
.equ	CPSR_MODE_IRQ,		0x12
.equ	CPSR_MODE_SVR,		0x13
.equ	CPSR_MODE_ABORT,	0x17
.equ	CPSR_MODE_UNDEFINED,	0x1B
.equ	CPSR_MODE_SYSTEM,	0x1F

// See ARM section A2.5 (Program status registers)
.equ	CPSR_IRQ_INHIBIT,	0x80
.equ	CPSR_FIQ_INHIBIT,	0x40
.equ	CPSR_THUMB,		0x20

.global prepare_interrupt_vector
prepare_interrupt_vector:
	push {r0, r1, lr}

	ldr	r0, =isr_vector
	ldr	r1, =0x0000
	ldmia	r0!, {r2-r9}
	stmia	r1!, {r2-r9}
	ldmia	r0!, {r2-r8}
	stmia	r1!, {r2-r8}

	// We're going to use interrupt mode, so setup the interrupt mode
	// stack pointer which differs to the application stack pointer:
	mov r0, #(CPSR_MODE_IRQ | CPSR_IRQ_INHIBIT | CPSR_FIQ_INHIBIT )
	msr cpsr_c, r0
	mov sp, #0x8000

	// Switch back to supervisor mode (our application mode)

	mov r0, #(CPSR_MODE_SVR | CPSR_IRQ_INHIBIT | CPSR_FIQ_INHIBIT )
	msr cpsr_c, r0

	pop {r0, r1, pc}

.global enable_interrupts
enable_interrupts:
	mrs r0, cpsr
	bic r0, r0, #0x80 //clear 7th bit to enable interrupts
	msr cpsr_c, r0

	bx lr
