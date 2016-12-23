#include "console.h"
void sys_api(int edi,int esi,int ebp,int esp,int ebx,int edx,int ecx,int eax)
{
	struct CONSOLE *cons = (struct CONSOLE *) *((int *)0x0fec);
	int cs_base = *((int *)0xfe8);
	if(edx == 1) cons_putchar(cons,eax & 0xff,1);
	else if(edx == 2) cons_putstr0(cons,(char *)ebx + cs_base);
	else if(edx == 3) cons_putstr1(cons,(char *)ebx + cs_base,ecx);
	return;
}

