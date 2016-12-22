#include "console.h"
void sys_api(int edi,int esi,int ebp,int esp,int ebx,int edx,int ecx,int eax)
{
	struct CONSOLE *cons = (struct CONSOLE *) *((int *)0x0fec);
	if(edx == 1) cons_putchar(cons,eax & 0xff,1);
	else if(edx == 2) cons_putstr0(cons,(char *)ebx);
	else if(edx == 3) cons_putstr1(cons,(char *)ebx,ecx);
	return;
}

