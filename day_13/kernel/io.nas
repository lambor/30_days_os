;[FORMAT "WCOFF"]
[BITS 32]

;[FILE "header.nas"]
	global io_hlt, io_cli, io_sti, io_stihlt
	global write_mem8
	global io_in8, io_in16, io_in32
	global io_out8, io_out16, io_out32
	global io_load_eflags,io_store_eflags
	global load_gdtr,load_idtr
	global load_cr0,store_cr0
	global _memtest_sub

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

load_cr0:			;int load_cr0(void);
	mov eax,cr0;
	ret

store_cr0:			;void store_cr0(int cr0);
	mov eax,[esp+4];
	mov cr0,eax;
	ret

_memtest_sub:		;unsigned int _memtest_sub(unsigned int start,unsigned int end)
	push edi
	push esi
	push ebx
	mov esi,0xaa55aa55
	mov edi,0x55aa55aa
	mov eax,[esp+12+4]		;12 = edi + esi + ebx; 4 = start
mts_loop:
	mov ebx,eax
	add ebx,0xffc
	mov edx,[ebx]			;store the old mem with edx
	mov [ebx],esi			;set the mem with 0xaa55aa55
	xor dword [ebx],0xffffffff
	cmp edi,[ebx]
	jne mts_qit
	xor dword [ebx],0xffffffff
	cmp esi,[ebx]
	jne mts_qit
	mov [ebx],edx			;reload the old mem from edx
	add eax,0x1000
	cmp eax,[esp+12+8]
	jbe  mts_loop
mts_ret:
	pop ebx
	pop esi
	pop edi
	ret

mts_qit:
	mov [ebx],edx
	pop ebx
	pop esi
	pop edi
	ret
