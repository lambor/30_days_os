[BITS 32]
	global bin_entry
	extern binmain,api_end
[SECTION .text]
bin_entry:
	call binmain
	call api_end
