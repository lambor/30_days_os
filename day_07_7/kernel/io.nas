;[FORMAT "WCOFF"]
[BITS 32]

;[FILE "header.nas"]
	global io_hlt, io_cli, io_sti, io_stihlt
	global write_mem8
	global io_in8, io_in16, io_in32
	global io_out8, io_out16, io_out32
	global io_load_eflags,io_store_eflags
	global load_gdtr,load_idtr

[SECTION .text]
io_hlt:		;void io_hlt(void);
	hlt
	ret

io_cli:		;void io_cli(void);
	cli
	ret

io_sti:		;void io_sti(void);
	sti
	ret

io_stihlt:	;void io_stihlt(void);
	sti
	hlt
	ret

write_mem8:	;void write_mem8(int addr,int data)
	mov ecx,[esp+4]
	mov al,[esp+8]
	mov [ecx],al
	ret

io_in8:		;int io_in8(int port);
	mov edx,[esp+4]
	mov eax,0
	in al,dx
	ret

io_in16:	;int io_in16(int port);
	mov edx,[esp+4]
	mov eax,0
	in ax,dx
	ret

io_in32:	;int io_in32(int port);
	mov edx,[esp+4]
	mov eax,0
	in eax,dx
	ret

io_out8:	;int io_in8(int port,int data);
	mov edx,[esp+4]
	mov eax,[esp+8]
	out dx,al
	ret

io_out16:	;int io_in16(int port,int data);
	mov edx,[esp+4]
	mov eax,[esp+8]
	out dx,ax
	ret

io_out32:	;int io_in32(int port,int data);
	mov edx,[esp+4]
	mov eax,[esp+8]
	out dx,eax
	ret

io_load_eflags:		;int io_store_eflags(void);
	pushfd
	pop eax
	ret

io_store_eflags:	;void io_store_eflags(int eflags)
	mov eax,[esp+4]
	push eax
	popfd
	ret

load_gdtr:			;void load_gdtr(int limit,int addr);
	mov ax,[esp+4]
	mov [esp+6],ax
	lgdt [esp+6]
	ret

load_idtr:			;void load_idtr(int limit,int addr);
	mov ax,[esp+4]
	mov [esp+6],ax;
	lidt [esp+6]
	ret
