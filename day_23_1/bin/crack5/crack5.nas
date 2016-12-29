global binmain
[BITS 32]
binmain:
	call 3*8:0xb02
	mov edx,4
	int 0x40
