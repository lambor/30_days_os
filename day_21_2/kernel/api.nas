[BITS 32]
	extern cons_putchar
	global asm_cons_putchar

[SECTION .text]
asm_cons_putchar:
	push 1
	and eax,0xff
	push eax
	push dword [0x0fec]
	call cons_putchar
	add esp,12
	retf

