global start
extern long_mode_start

extern enable_paging

section .text
bits 32
start:
	mov esp, stack_top
	mov edi, ebx       ;Multiboot address

	call enable_paging
	
	lgdt [gdt64.pointer]
                
	; update selectors
	mov ax, gdt64.data
	mov ss, ax
	mov ds, ax
	mov es, ax

	jmp gdt64.code:long_mode_start

section .bss
align 4096
stack_bottom:
	resb 4096
stack_top:


section .rodata
gdt64:
	dq 0								; zero entry
.code: equ $ - gdt64
	    dq (1 << 44) | (1 << 47) | (1 << 41) | ( 1 << 43) | (1 << 53)	; code segment
.data: equ $ - gdt64
            dq (1 << 44) | (1 << 47) | (1 << 41) 				; data segmenty.
.pointer:
	dw $ - gdt64 - 1
	dq gdt64