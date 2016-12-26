[BITS 32]
	global bin_entry
	extern binmain
[SECTION .text]
	call binmain
	retf
