[BITS 32]
	global start_app
[SECTION .text]
start_app:		;void start_app(int eip,int cs,int esp,int ds,int *tss_esp0);
	pushad
	mov eax,[esp+36]
	mov ecx,[esp+40]
	mov edx,[esp+44]
	mov ebx,[esp+48]
	mov [0xfe4],esp
	cli
	mov es,bx
	mov ss,bx
	mov ds,bx
	mov fs,bx
	mov gs,bx
	mov esp,edx
	sti
	push ecx
	push eax
	call far [esp]
;return to system
	mov eax,1*8
	cli
	mov es,ax
	mov ss,ax
	mov ds,ax
	mov fs,ax
	mov gs,ax
	mov esp,[0xfe4]
	sti
	popad
	ret
	
