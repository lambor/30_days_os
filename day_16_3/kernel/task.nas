[BITS 32]
	global load_tr 		;void load_tr(int tr);
	global farjmp		;void farjmp(int eip,int cs);
[SECTION .text]

load_tr:
	ltr [esp + 4]
	ret

farjmp:			;void farjmp(int eip,int cs);
	jmp far [esp + 4]
	ret
