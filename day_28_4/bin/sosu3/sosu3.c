#include "../api.h"

#define MAX 10000

void binmain(void)
{
	char flag[MAX],s[8];
	int i,j;
	for(i=0;i<MAX;i++)
		flag[i] = 0;

	for(i=2;i<MAX;i++)
	{
		if(flag[i] == 0)
		{
			sprintf(s,"%d ",i);
			api_putstr(s);
		
		for(j=i*2;j<MAX;j+=i)
			flag[j] = i;
		//api_getkey(1);
		}
	}
}
