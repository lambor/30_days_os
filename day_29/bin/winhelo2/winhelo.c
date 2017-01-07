#include "../api.h"
void binmain(void)
{
	char *buf;
	int win;

	api_initmalloc();
	buf = api_malloc(150*50);
	win = api_openwin(buf,150,50,-1,"hello");
	api_boxfilwin(win,8,36,141,43,6);
	char s[6] = {"hello"};
	s[5] = 0;
	api_putstrwin(win,28,28,0,12,s);
	api_getkey(1);
	return;
}
