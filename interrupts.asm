global enable_interrupts

section .text
bits 64
enable_interrupts:
	; Save to make following instructions smaller
	mov rdi, IDT
	
	; Set an IDT entry to a callback
	%macro SETIDT 2
	mov rsi, %1
	mov rax, %2
	call set_idt
	%endmacro
	
	; Install error handlers
	%assign i 0
	mov rsi, 0
	%rep 32
	mov rax, Isr%[i]
	call set_idt
	inc rsi
	%assign i i+1
	%endrep
	; Install stub IRQs
	%assign i	32
	%rep 128-32
	mov rax, Irq%[i]
	call set_idt
	inc rsi
	%assign i i+1
	%endrep
	
	mov rdi, IDTPtr
	lidt [rdi]

	ret
; - Custom CC:
; RDI = IDT
; RSI = Index
; RAX = Address
set_idt:
	shl rsi, 4
	mov WORD [rdi + rsi], ax
	shr rax, 16
	mov WORD [rdi + rsi + 6], ax
	shr rax, 16
	mov DWORD [rdi + rsi + 8], eax
	; Enable
	mov ax, WORD [rdi + rsi + 4]
	or  ax, 0x8000
	mov WORD [rdi + rsi + 4], ax
	shr rsi, 4
	ret

ErrorCommon:
    pop rax
    pop rax
    mov [0xb8000], al
    ;add rsp, 1*8
    iretq
    
IRQCommon:
    pop rbx
    iretq

%macro ISR_NOERRNO	1
Isr%1:
	push	QWORD 0
	push	QWORD %1
	jmp	ErrorCommon
%endmacro
%macro ISR_ERRNO	1
Isr%1:
	push	QWORD %1
	jmp	ErrorCommon
%endmacro

ISR_NOERRNO	0;  0: Divide By Zero Exception
ISR_NOERRNO	1;  1: Debug Exception
ISR_NOERRNO	2;  2: Non Maskable Interrupt Exception
ISR_NOERRNO	3;  3: Int 3 Exception
ISR_NOERRNO	4;  4: INTO Exception
ISR_NOERRNO	5;  5: Out of Bounds Exception
ISR_NOERRNO	6;  6: Invalid Opcode Exception
ISR_NOERRNO	7;  7: Coprocessor Not Available Exception
ISR_ERRNO	8;  8: Double Fault Exception (With Error Code!)
ISR_NOERRNO	9;  9: Coprocessor Segment Overrun Exception
ISR_ERRNO	10; 10: Bad TSS Exception (With Error Code!)
ISR_ERRNO	11; 11: Segment Not Present Exception (With Error Code!)
ISR_ERRNO	12; 12: Stack Fault Exception (With Error Code!)
ISR_ERRNO	13; 13: General Protection Fault Exception (With Error Code!)
ISR_ERRNO	14; 14: Page Fault Exception (With Error Code!)
ISR_NOERRNO	15; 15: Reserved Exception
ISR_NOERRNO	16; 16: Floating Point Exception
ISR_NOERRNO	17; 17: Alignment Check Exception
ISR_NOERRNO	18; 18: Machine Check Exception
ISR_NOERRNO	19; 19: Reserved
ISR_NOERRNO	20; 20: Reserved
ISR_NOERRNO	21; 21: Reserved
ISR_NOERRNO	22; 22: Reserved
ISR_NOERRNO	23; 23: Reserved
ISR_NOERRNO	24; 24: Reserved
ISR_NOERRNO	25; 25: Reserved
ISR_NOERRNO	26; 26: Reserved
ISR_NOERRNO	27; 27: Reserved
ISR_NOERRNO	28; 28: Reserved
ISR_NOERRNO	29; 29: Reserved
ISR_NOERRNO	30; 30: Reserved
ISR_NOERRNO	31; 31: Reserved

%macro BLANKINT 1
Irq%1:
	push rbx
	mov rbx, %1
	jmp IRQCommon
%endmacro

%assign i	32
%rep 128-32
BLANKINT i
%assign i i+1
%endrep

IDT:
        times 256	dd	0x00080000, 0x00000E00, 0, 0
IDTPtr:
	dw	256*16-1
	dq	IDT