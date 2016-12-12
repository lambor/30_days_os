;[FORMAT "WCOFF"]
[BITS 32]

;[FILE "header.nas"]
	global io_hlt

[SECTION .text]
io_hlt:		;void io_hlt(void);
	hlt
	ret

