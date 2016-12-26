[BITS 32]
	global api_putchar,api_putstr

[SECTION .text]
api_putchar:	;void api_putchar(int c);
	mov edx,1
	mov al,[esp+4]
	int 0x40
	ret

api_putstr:		;void api_putstr(char *str);
	mov edx,2
	mov ebx,[esp+4]
	int 0x40
	ret
