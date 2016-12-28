	global binmain
[BITS 32]
binmain:
	mov eax,1*8
	mov ds,ax
	mov byte [0x102600],0
	ret
