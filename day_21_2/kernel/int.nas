;[FORMAT "WCOFF"]
[BITS 32]

;[FILE "header.nas"]
	extern inthandler21,inthandler2c,inthandler20
	global asm_inthandler21,asm_inthandler2c,asm_inthandler20
	extern sys_api
	global asm_sys_api

[SECTION .text]
asm_inthandler21:
	push es
	push ds
	pushad
	mov eax,esp
	push eax
	mov ax,ss
	mov ds,ax
	mov es,ax
	call inthandler21
	pop eax
	popad
	pop ds
	pop es
	iretd

asm_inthandler2c:
	push es
	push ds
	pushad
	mov eax,esp
	push eax
	mov ax,ss
	mov ds,ax
	mov es,ax
	call inthandler2c
	pop eax
	popad
	pop ds
	pop es
	iretd

asm_inthandler20:
	push es
	push ds
	pushad
	mov eax,esp
	push eax
	mov ax,ss
	mov ds,ax
	mov es,ax
	call inthandler20
	pop eax
	popad
	pop ds
	pop es
	iretd

asm_sys_api:
	sti
	pushad
	pushad
	call sys_api
	add esp,32
	popad
	iretd
