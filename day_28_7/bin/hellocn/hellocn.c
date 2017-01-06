#include "../api.h"
void binmain(void)
{
	int langmode  = api_getlang();
	static unsigned char hello_txt[] = {
		  0xc4, 0xe3, 0xba, 0xc3, 0xca, 0xc0, 0xbd, 0xe7, 0x00
	};
	if(langmode == 0)
		api_putstr("hello world\n");
	else if(langmode == 1)
		api_putstr(hello_txt);
}
