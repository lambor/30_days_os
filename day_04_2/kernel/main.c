#include "header.h"
int bootmain()
{
	int i;
	for(i=0xa0000;i<=0xaffff;i++)
	{
		write_mem8(i,i&0x0f);
	}

	for(;;)io_hlt();
}
