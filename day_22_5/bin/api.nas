[BITS 32]
	global api_putchar,api_putstr
	global api_end
	global api_test

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

api_end:		;void api_end(void);
	mov edx,4
	int 0x40

api_test:
	add eax,0x80
	ret
