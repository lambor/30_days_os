#include "header.h"
int bootmain()
{
	init_gdtidt();
	//entry.S write the boot info into 0x0ff0
	struct BOOTINFO *binfo = (struct BOOTINFO *)0xff0;
	
	init_palette();
	init_screen(binfo->vram,binfo->scrnx,binfo->scrny);
	
	putfonts8_asc(binfo->vram,binfo->scrnx,8,8,COL8_FFFFFF,"ABC 123");
	putfonts8_asc(binfo->vram,binfo->scrnx,31,31,COL8_000000,"dcnh os.");
	putfonts8_asc(binfo->vram,binfo->scrnx,30,30,COL8_FFFFFF,"dcnh os.");

	char mouse[256] = {0};
	init_mouse_cursor8(mouse,COL8_008484);
	int mx = 160,my =100;
	putblock8_8(binfo->vram,binfo->scrnx,16,16,mx,my,mouse,16);
	/*
	char s[20] = {0};
	sprintf(s,"scrnx = %d",binfo->scrnx);
	putfonts8_asc(binfo->vram,binfo->scrnx,16,64,COL8_FFFFFF,s);
	*/
	for(;;)io_hlt();
}
