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
	;usually from user app
	;cs = 3*8 (sys); ds,ss... = 1004*8(usr)
	push ds
	push es
	pushad
	mov eax,1*8
	mov ds,ax
	mov ecx,[0xfe4]	;reload sys esp
	add ecx,-40		;rebase the "sys stack"
	mov [ecx+32],esp;store usr esp in "sys stack"
	mov [ecx+36],ss	;store usr ss in "sys stack"
	
	;"push" arguments that passed by usr_api using pushad into "sys stack"
	mov edx,[esp	]
	mov ebx,[esp + 4]
	mov [ecx	],edx
	mov [ecx + 4],ebx
	mov edx,[esp + 8]
	mov ebx,[esp +12]
	mov [ecx + 8],edx
	mov [ecx +12],ebx
	mov edx,[esp +16]
	mov ebx,[esp +20]
	mov [ecx +16],edx
	mov [ecx +20],ebx
	mov edx,[esp +24]
	mov ebx,[esp +28]
	mov [ecx +24],edx
	mov [ecx +28],ebx

	;init data segments before into sys
	mov es,ax
	mov ss,ax
	mov esp,ecx
	sti
	
	;into sys
	call sys_api
	
	;restore data segments back from sys
	mov ecx,[esp +32]
	mov eax,[esp +36]
	cli
	mov ss,ax
	mov esp,ecx
	popad
	pop es
	pop ds
	iretd

asm_inthandler0d:
	;from user app or sys
	;cs = 3*8 (sys); ds,ss... = 1*8(sys) or 1004*8(usr)
	sti
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
	call inthandler0d
	
	add esp,8
	popad
	pop ds
	pop es
	add esp,4
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
	sti
	call inthandler0d
	cli
	cmp eax,0
	jne .kill
	pop ecx
	pop eax
	mov ss,ax
	mov esp,ecx
	popad
	pop ds
	pop es
	add esp,4
	iretd
.kill:
	mov eax,1*8
	mov es,ax
	mov ss,ax
	mov ds,ax
	mov fs,ax
	mov gs,ax
	mov esp,[0xfe4]
	sti
	popad
	ret
