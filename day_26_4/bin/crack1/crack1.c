#include "../api.h"
void binmain(void)
{
	*((char *)0x00102600) = 0;
	return;
}
