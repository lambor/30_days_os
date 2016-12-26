;[FORMAT "WCOFF"]
[BITS 32]

;[FILE "header.nas"]
	extern inthandler21,inthandler2c,inthandler20
	global asm_inthandler21,asm_inthandler2c,asm_inthandler20
	extern sys_api
	global asm_sys_api
	extern inthandler0d
	global asm_inthandler0d

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
	;from user app or sys
	;cs = 3*8 (sys); ds,ss... = 1*8(sys) or 1004*8(usr)
	push es
	push ds
	pushad
	mov ax,ss
	cmp ax,1*8
	jne .from_app

	mov eax,esp
	push ss
	push eax
	mov ax,ss
	mov ds,ax
	mov es,ax
	call inthandler20
	
	add esp,8
	popad
	pop ds
	pop es
	iretd
.from_app:
	;if from user app
	;cs = 3*8 but ds,ss... = 1004*8
	mov eax,1*8
	mov ds,ax
	mov ecx,[0xfe4]	;reload sys esp
	add ecx,-8		;push arguments 
	mov [ecx+4],ss	;push argument ss
	mov [ecx],esp	;push argument esp
	mov ss,ax
	mov es,ax
	mov esp,ecx
	call inthandler20
	
	pop ecx
	pop eax
	mov ss,ax
	mov esp,ecx
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
	mov ds,ax
	call inthandler0d
	cmp eax,0
	jne end_app
	pop eax
	popad
	pop ds
	pop es
	add esp,4
	iretd
