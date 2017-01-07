#include "../api.h"
#include "../string.h"
void binmain(void)
{
	char s[10];
	sprintf(s,"%d",strlen("hello"));
	api_putstr(s);
	return;
}
