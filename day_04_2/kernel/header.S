.section .text
	.global io_hlt
	;.type io_hlt,@function
	.global test_screen
	;.type test_screen,@function

io_hlt:
	hlt
	ret

test_screen:
	mov $0x13,%al
	mov $0x00,%ah
	int $0x10
	ret
