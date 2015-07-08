global swtch
swtch:
	mov eax, [esp + 4]	; old stack ptr
	mov edx, [esp + 8]	; new stack ptr

	pushfd			; push regs to current ctx
	push ebp
	push ebx
	push esi
	push edi

	mov [eax], esp		; update old ctx ptr with current stack ptr
	mov esp, edx		; switch to new stack

	pop edi			; pop saved stack of previous task
	pop esi
	pop ebx
	pop ebp
	popfd

	ret

global read_esp
read_esp:
	mov eax, esp
	ret
