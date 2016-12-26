[BITS 32]
	global bin_entry
	extern binmain
[SECTION .text]
bin_entry:
	call binmain
	retf
