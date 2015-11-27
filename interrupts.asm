global enable_interrupts

section .text
bits 64
remap_PIC:
    in al, 0x21                   ; save pic1 mask
    mov cl, al
    in al, 0xA1                   ; save pic2 mask
    mov ch, al

    mov al, 0x11
    out 0x20, al                ; send initialize command to pic1
    out 0xA0, al                ; send initialize command to pic2

    mov al, 0x20
    out 0x21, al                ; set vector offset of pic1 to 0x20
    mov al, 0x28
    out 0xA1, al                ; set vector offset of pic2 to 0x28

    mov al, 4
    out 0x21, al                   ; tell pic1 that there is a slave PIC at IRQ2 (0000 0100)
    mov al, 2
    out 0xA1, al                   ; tell pic2 its cascade identity (0000 0010)

    mov al, 0x1
    out 0x21, al                 ; 8086 mode for pic1
    out 0xA1, al                 ; 8086 mode for pic2

    mov al, cl
    out 0x21, al                  ; restore pic1 mask
    mov al, ch
    out 0xA1, al                  ; restore pic2 mask

    ret

enable_interrupts:
	call remap_PIC
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
    ;mov [0xb8000], al
    ;mov [0xb8000], word 0x4234243
    ;add rsp, 1*8
    iretq
    
IRQCommon:

    pop rbx
    add bx, '0'
    mov [0xb8000], bx
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
