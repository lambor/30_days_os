[BITS 32]
	global binmain
[SECTION .text]
binmain:
	mov ax,1005*8;
	mov ds,ax
	cmp dword [ds:0x0004],'Hari'
	jne fin

	mov ecx,[ds:0x0000]
	mov ax,2005*8
	mov ds,ax

crackloop:
	add ecx,-1
	mov byte [ds:ecx],123
	cmp ecx,0
	jne crackloop

fin:
	mov edx,4
	int 0x40


