#include "../api.h"
void binmain(void)
{
	int langmode  = api_getlang();
	static unsigned char hello_txt[] = {
		  0xc4, 0xe3, 0xc4, 0xe4, 0xc4, 0xe5, 0xc4, 0xe6, 0x00
	};
	if(langmode == 0)
		api_putstr("hello world\n");
	else if(langmode == 1)
		api_putstr(hello_txt);
}
