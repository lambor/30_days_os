#include "../api.h"

char buf[150*70];
void binmain(void)
{
	int win;
	win = api_openwin(buf,150,70,255,"notrec");
	api_boxfilwin(win,0,50,34,69,255);
	api_boxfilwin(win,115,50,149,69,255);
	api_boxfilwin(win,50,30,99,49,255);
	for(;;)
	{
		if(api_getkey(1) == 0x0a)
			break;
	}
	api_end();
}