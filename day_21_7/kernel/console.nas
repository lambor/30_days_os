[BITS 32]
	global start_app
[SECTION .text]
start_app:		;void start_app(int eip,int cs,int esp,int ds,int *tss_esp0);
	pushad
	mov eax,[esp+36]
	mov ecx,[esp+40]
	mov edx,[esp+44]
	mov ebx,[esp+48]
	mov ebp,[esp+52]
	mov [ebp	],esp
	mov [ebp + 4],ss
	mov es,bx
	mov ds,bx
	mov fs,bx
	mov gs,bx
	or ecx,3
	or ebx,3
	push ebx
	push edx
	push ecx
	push eax
	retf
