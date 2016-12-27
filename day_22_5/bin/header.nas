dd 64*1024
db "Hari"
dd 0
dd	;stack esp
dd 	;data section size
dd  ;data section start point
dd 0xe9000000	;jmp
dd 	;code start point - 0x20
dd  ;malloc start point
