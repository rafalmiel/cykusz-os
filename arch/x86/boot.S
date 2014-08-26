global _loader
extern kernel_main

MODULEALIGN		equ	1<<0
MEMINFO			equ	1<<1
FLAGS			equ	MODULEALIGN | MEMINFO
MAGIC			equ	0x1BADB002
CHECKSUM		equ	-(MAGIC + FLAGS)

KERNEL_VIRTUAL_BASE	equ	0xC0000000			; 3GB
KERNEL_PAGE_NUMBER	equ	(KERNEL_VIRTUAL_BASE >> 22)	; Page directory index of kernels page entries

section .data
align 0x1000
global BootPageEntry
BootPageEntry:
	; Page entries, will contain identity mapping of the initial kernel data
	times 1024 * 256 dd 0

align 0x1000
global BootPageDirectory
BootPageDirectory:
	; Page directory placeholder, will contain pointers to BootPageEntry
	; at page nr 0 and KERNEL_PAGE_NUMBER
	times 1024 dd 0

section .text
align 4
MultiBootHeader:
	dd MAGIC
	dd FLAGS
	dd CHECKSUM

; reserve initial kernel stack space -- thats 16k.
STACKSIZE equ 0x4000

; setting up entry point for linker
global loader
loader equ (_loader - KERNEL_VIRTUAL_BASE)

extern __end
extern __phys_end

; Setup BootPageDirectory mapping (put pointers to BootPageEntry at appropriate
; places
fill_page_directory:
	push eax
	push ebx
	push ecx

	mov eax, BootPageDirectory - KERNEL_VIRTUAL_BASE
	mov ebx, BootPageEntry - KERNEL_VIRTUAL_BASE
	mov ecx, 0
	add ebx, 0x7

fill_page_directory_loop:
	mov [eax], ebx
	mov [eax + KERNEL_PAGE_NUMBER*4], ebx

	add eax, 4			; Move index in BootPageDirectory
	add ebx, 1024 * 4		; Move pointer to BootPageEntry
	add ecx, 1			; Inc counter index

	cmp ecx, 256			; Stop after mapping 256 entries
	jl fill_page_directory_loop

	pop ecx
	pop ebx
	pop eax
	ret

; Setup identity mapping of the kernel data
fill_page_entry:
	push eax
	push ebx
	push ecx
	push edx

	mov edx, BootPageEntry - KERNEL_VIRTUAL_BASE
	mov ebx, __end - KERNEL_VIRTUAL_BASE
	mov ecx, 0
	mov eax, 0x7					; Flags
fill_page_entry_loop:
	mov [edx + ecx], eax

	add eax, 0x1000
	add ecx, 0x4

	cmp eax, ebx
	jle fill_page_entry_loop

	pop edx
	pop ecx
	pop ebx
	pop eax

	ret

_loader:
	call fill_page_directory
	call fill_page_entry

	mov ecx, (BootPageDirectory - KERNEL_VIRTUAL_BASE)
	mov cr3, ecx					; Set address of BPD

	mov ecx, cr0
	or ecx, 0x80000000				; Enable paging
	mov cr0, ecx

	lea ecx, [StartInHigherHalf]
	jmp ecx						; Jmp to higher half kernel

StartInHigherHalf:
	mov dword [BootPageDirectory], 0		; Remove mapping of first ~1MB
	invlpg [0]

	mov esp, stack+STACKSIZE			; set up the stack
	call  kernel_main				; call kernel proper

.hang:
	hlt						; halt machine should kernel return
	jmp .hang


section .bss
align 32
stack:
	resb STACKSIZE					; reserve 16k stack on a quadword boundary
