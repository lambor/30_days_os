dd DATA_SEG_SIZE
db "Hari"
dd DATA_SEG_RESV
dd STACK_ESP0			;stack esp
dd DATA_SEC_SIZE		;data section size
dd CODE_SEC_SIZE + 64	;data section start point
dd 0xe9000000			;jmp
dd HEADER_SIZE - 0x20	;code start point - 0x20
dd HEAP_BEGIN			;malloc start point
times HEADER_SIZE-($-$$) db 0
