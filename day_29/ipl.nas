;hello-os
;TAB = 4

	org 0x7c00
	jmp entry

;FAT12 FORMAT
	db 0x90
	db "helloipl"	;name of start part
	dw 512			;sector size
	db 1			;cluster size
	dw 1			;FAT start position
	db 2			;FAT num must be 2
	dw 224 			;root dir size
	dw 2880			;disk size
	db 0xf0			;disk type
	dw 9			;FAT length must be 9 sections
	dw 18			;section num per track
	dw 2			;mag head num
	dd 0			;must use segment
	dd 2880			;rewrite disk size
	db 0,0,0x29		;unknown
	dd 0xffffffff	
	db "HELLO-OS   ";disk name
	db "FAT12   "	;disk format name
	;resb 18			;reserve 18 bytes

;the body of ipl
	CYLS equ 20
	
entry:
	mov ax,0
	mov ss,ax
	mov sp,0x7c00
	mov ds,ax
	mov es,ax

	mov si,msg
	call putloop

load:
	mov ax,0x820	;load dest memory position
	mov es,ax
	mov ch,0
	mov dh,0
	mov cl,2
readloop:
	mov si,0
retry:
	mov ah,0x02
	mov al,1
	mov bx,0
	mov dl,0
	int 0x13
	jnc next	;read suc
	add si,1
	cmp si,5	;max retry num
	jae error   ;read fail after 5 try
	mov ah,0
	mov dl,0
	int 0x13
	jmp retry
next:
	mov ax,es
	add ax,0x20
	mov es,ax
	add cl,1
	cmp cl,18
	jbe readloop
	mov cl,1
	add dh,1
	cmp dh,2
	jb readloop
	mov dh,0
	add ch,1
	cmp ch,CYLS
	jb readloop

	mov [0x0ff0],ch ;for entry.S CYLS
	jmp fin

putloop:
	mov al,[si]
	add si,1
	cmp al,0
	je over
	mov ah,0x0e
	mov bx,15
	int 0x10
	call putloop

over:
	ret

fin:
	int 0x3
	jmp 0xc400

error:
	mov si,errormsg
	jmp putloop

msg:
	db 0x0a,0x0a
	db "hello,world"
	db 0x0a
	db 0
	
errormsg:
	db 0x0a,0x0a
	db "load error"
	db 0x0a
	db 0

	times 510-($-$$) db 0
	db 0x55,0xaa
