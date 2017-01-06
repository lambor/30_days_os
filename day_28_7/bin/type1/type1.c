#include "../api.h"
void binmain(void)
{
	int fh;
	char c;
	fh = api_fopen("1111.txt");
	if(fh != 0)
	{
		for(;;) 
		{
			if(api_fread(&c,1,fh) == 0)
				break;
			api_putchar(c);
		}
	}
}
