;hello-os
;TAB = 4
	org 0x7c00
	jmp entry
;FAT12 FORMAT
	db 0x90			;magic num
	db "helloipl" 	;name of start part
	dw 512        	;sector size
	db 1          	;cluster size
	dw 1          	;FAT start position
	db 2          	;FAT num must be 2
	dw 224        	;root dir size
	dw 2880			;disk size
	dw 0xf0			;disk type
	dw 9			;FAT length must be 9 sections
	dw 18			;section num per track
	dw 2			;mag head num
	dd 0			;must use segment
	dd 2880			;rewrite disk size
	db 0,0,0x29		;unknown
	db "HELLO-OS   ";disk name
	db "FAT12   "	;disk format name
	resb 18			;reserve 18 bytes

;the body of ipl
entry:
	mov ax,0
	mov ss,ax
	mov sp,0x7c00
	mov ds,ax
	mov es,ax

	mov si,msg

putloop:
	mov al,[si]
	add si,1
	cmp al,0
	
	je fin
	mov ah,0x0e
	mov bx,15
	int 0x10
	jmp putloop

fin:
	hlt
	jmp fin

msg:
	db 0x0a,0x0a
	db "hello,world"
	db 0x0a
	db 0
	
	times 510-($-$$) db 0
	db 0x55,0xaa

;the rest of the disk
	db 0xf0,0xff,0xff,0x00,0x00,0x00,0x00,0x00
	resb 4600
	db 0xf0,0xff,0xff,0x00,0x00,0x00,0x00,0x00
	resb 1469432
