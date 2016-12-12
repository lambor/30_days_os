#include "header.h"
int bootmain()
{
	char *vram;
	int xsize,ysize;
	init_palette();
	vram=(char*)0xa0000;
	xsize=320;
	ysize=200;
	
	boxfill8(vram,xsize,COL8_008484,0,		0,			xsize-1,	ysize-29);
	boxfill8(vram,xsize,COL8_C6C6C6,0,		ysize-28,	xsize-1,	ysize-28);
	boxfill8(vram,xsize,COL8_FFFFFF,0,		ysize-27,	xsize-1,	ysize-27);
	boxfill8(vram,xsize,COL8_C6C6C6,0,		ysize-26,	xsize-1,	ysize- 1);
	

	boxfill8(vram,xsize,COL8_FFFFFF,3,		ysize-24,	59,			ysize-24);
	boxfill8(vram,xsize,COL8_FFFFFF,2,		ysize-24,	 2,			ysize- 4);
	boxfill8(vram,xsize,COL8_848484,3,		ysize- 4,	59,			ysize- 4);
	boxfill8(vram,xsize,COL8_848484,59,		ysize-23,	59,			ysize- 5);
	boxfill8(vram,xsize,COL8_000000,2,		ysize- 3,	59,			ysize- 3);
	boxfill8(vram,xsize,COL8_000000,60,		ysize-24,	60,			ysize- 3);

	boxfill8(vram,xsize,COL8_848484,xsize-47,		ysize-24,	xsize- 4,	ysize-24);
	boxfill8(vram,xsize,COL8_848484,xsize-47,		ysize-23,	xsize-47,	ysize- 4);
	boxfill8(vram,xsize,COL8_FFFFFF,xsize-47,		ysize- 3,	xsize- 4,	ysize- 3);
	boxfill8(vram,xsize,COL8_FFFFFF,xsize- 3,		ysize-24,	xsize- 3,	ysize- 3);
	
	/*
	for(int i=0;i<=0xffff;i++)
	{
		vram[i] = i&0x0f;
	}
	*/

	for(;;)io_hlt();
}
