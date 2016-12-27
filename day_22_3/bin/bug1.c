#include "api.h"
void binmain(void)
{
	char a[100];
	a[10] = 'A';
	api_putchar(a[10]);
	a[102] = 'B';
	api_putchar(a[102]);
	a[123] = 'C';
	api_putchar(a[123]);
}
