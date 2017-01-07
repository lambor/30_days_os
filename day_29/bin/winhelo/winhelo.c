#include "../api.h"
char buf[150*50];
void binmain(void)
{
	int win;
	win = api_openwin(buf,150,50,-1,"hello");
	api_boxfilwin(win,8,36,141,43,3);
	api_putstrwin(win,28,28,0,12,"hello world");
	api_getkey(1);
}
