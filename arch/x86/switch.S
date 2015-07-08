global swtch
swtch:
	mov eax, [esp + 4]	; old stack ptr
	mov edx, [esp + 8]	; new stack ptr

	pushfd
	push ebp
	push ebx
	push esi
	push edi

	mov [eax], esp
	mov esp, edx

	pop edi
	pop esi
	pop ebx
	pop ebp
	popfd
	ret

global read_esp
read_esp:
	mov eax, esp
	ret
