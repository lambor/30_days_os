;[FORMAT "WCOFF"]
[BITS 32]

;[FILE "header.nas"]
	extern inthandler21,inthandler2c,inthandler20
	global asm_inthandler21,asm_inthandler2c,asm_inthandler20
	extern sys_api
	global asm_sys_api
	extern inthandler0d
	global asm_inthandler0d
	extern inthandler0c
	global asm_inthandler0c

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
	push ds
	push es
	pushad
	pushad
	mov ax,ss
	mov ds,ax
	mov es,ax
	call sys_api
	cmp eax,0
	jne end_app
	add esp,32
	popad
	pop es
	pop ds
	iretd
end_app:
	mov esp,[eax]
	popad
	ret

asm_inthandler0d:
	sti
	push es
	push ds
	pushad
	mov eax,esp
	push eax
	mov ax,ss
	mov ds,ax
	mov es,ax
	call inthandler0d
	cmp eax,0
	jne end_app
	pop eax
	popad
	pop ds
	pop es
	add esp,4
	iretd

asm_inthandler0c:
	sti
	push es
	push ds
	pushad
	mov eax,esp
	push eax
	mov ax,ss
	mov ds,ax
	mov es,ax
	call inthandler0c
	cmp eax,0
	jne end_app
	pop eax
	popad
	pop ds
	pop es
	add esp,4
	iretd

