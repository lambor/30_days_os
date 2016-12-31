[BITS 32]
	global api_putchar,api_putstr
	global api_end
	global api_test
	global api_openwin,api_putstrwin,api_boxfilwin,api_refreshwin,api_closewin
	global api_initmalloc,api_malloc,api_free
	global api_pointwin,api_linewin
	global api_getkey
	global api_alloctimer,api_inittimer,api_settimer,api_freetimer

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

api_openwin:	;int api_openwin(char *buf,int xsiz,int ysiz,int col_inv,char *title);
	push edi
	push esi
	push ebx
	mov edx,5
	mov ebx,[esp + 16]	;buf
	mov esi,[esp + 20]	;xsiz
	mov edi,[esp + 24]	;ysiz
	mov eax,[esp + 28]	;col_inv
	mov ecx,[esp + 32]	;title
	int 0x40
	pop ebx
	pop esi
	pop edi
	ret

api_putstrwin:	;void api_putstrwin(int win,int x,int y,int col,int len,char *str);
	push edi
	push esi
	push ebp
	push ebx
	mov edx,6
	mov ebx,[esp + 20]
	mov esi,[esp + 24]
	mov edi,[esp + 28]
	mov eax,[esp + 32]
	mov ecx,[esp + 36]
	mov ebp,[esp + 40]
	int 0x40
	pop ebx
	pop ebp
	pop esi
	pop edi
	ret

api_boxfilwin:	;void api_boxfilwin(int win,int x0,int y0,int x1,int y1,int col);
	push edi
	push esi
	push ebp
	push ebx
	mov edx,7
	mov ebx,[esp+20]
	mov eax,[esp+24]
	mov ecx,[esp+28]
	mov esi,[esp+32]
	mov edi,[esp+36]
	mov ebp,[esp+40]
	int 0x40
	pop ebx
	pop ebp
	pop esi
	pop edi
	ret

api_initmalloc:	;void api_initmalloc(void);
	push ebx
	mov edx,8
	mov ebx,[cs:0x0020]
	mov eax,ebx
	add eax,32*1024		;init with 32k
	mov ecx,[cs:0x0000]
	sub ecx,eax
	int 0x40
	pop ebx
	ret

api_malloc:		;char *api_malloc(int size);
	push ebx
	mov edx,9
	mov ebx,[cs:0x0020]
	mov ecx,[esp+8]
	int 0x40
	pop ebx
	ret

api_free:		;void api_free(char *addr,int size);
	push ebx
	mov edx,10
	mov ebx,[cs:0x0020]
	mov eax,[esp+8]
	mov ecx,[esp+12]
	int 0x40
	pop ebx
	ret

api_pointwin:	;void api_pointwin(int win,int x,int y,int col);
	push edi
	push esi
	push ebx
	mov edx,11
	mov ebx,[esp+16]
	mov esi,[esp+20]
	mov edi,[esp+24]
	mov eax,[esp+28]
	int 0x40
	pop ebx
	pop esi
	pop edi
	ret

api_refreshwin:	;void api_refreshwin(int win,int x0,int y0,int x1,int y1);
	push edi
	push esi
	push ebx
	mov edx,12
	mov ebx,[esp+16]
	mov eax,[esp+20]
	mov ecx,[esp+24]
	mov esi,[esp+28]
	mov edi,[esp+32]
	int 0x40
	pop ebx
	pop esi
	pop edi
	ret


api_linewin:	;void api_linewin(int win,int x0,int y0,int x1,int y1,int col);
	push edi
	push esi
	push ebp
	push ebx
	mov edx,13
	mov ebx,[esp+20]
	mov eax,[esp+24]
	mov ecx,[esp+28]
	mov esi,[esp+32]
	mov edi,[esp+36]
	mov ebp,[esp+40]
	int 0x40
	pop ebx
	pop ebp
	pop esi
	pop edi
	ret

api_closewin:	;void api_closewin(int win);
	push ebx
	mov edx,14
	mov ebx,[esp+8]
	int 0x40
	pop ebx
	ret

api_getkey:		;int api_getkey(int mode);
	mov edx,15
	mov eax,[esp+4]
	int 0x40
	ret

api_alloctimer:
	mov edx,16
	int 0x40
	ret

api_inittimer:
	push ebx
	mov edx,17
	mov ebx,[esp+8]
	mov eax,[esp+12]
	int 0x40
	pop ebx
	ret

api_settimer:
	push ebx
	mov edx,18
	mov ebx,[esp+8]
	mov eax,[esp+12]
	int 0x40
	pop ebx
	ret

api_freetimer:
	push ebx
	mov edx,19
	mov ebx,[esp+8]
	int 0x40
	pop ebx
	ret
