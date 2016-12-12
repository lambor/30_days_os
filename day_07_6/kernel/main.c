#include "header.h"

int bootmain()
{

	init_gdtidt();
	init_pic();
	io_sti();

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
	
	
	io_out8(PIC0_IMR,0xf9);
	io_out8(PIC1_IMR,0xef);

	extern struct FIFO8 keybuf;
	char _keybuf[32] = {0};
	char s[8] = {0};
	fifo8_init(&keybuf,32,_keybuf);
	for(;;)
	{
		io_cli();
		if(fifo8_status(&keybuf) == 0)
		{
			io_stihlt();
		}
		else
		{
			unsigned char i = fifo8_get(&keybuf);
			io_sti();
			xtoa(i,s);
			boxfill8(binfo->vram,binfo->scrnx,COL8_000000,0,16,31,31);
			putfonts8_asc(binfo->vram,binfo->scrnx,0,16,COL8_FFFFFF,s);
		}
	}
}
