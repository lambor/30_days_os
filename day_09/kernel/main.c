#include "header.h"

int bootmain()
{

	init_gdtidt();
	init_pic();
	io_sti();

	init_keyboard();
	struct MOUSE_DEC mdec = {0};
	enable_mouse(&mdec);

	//init memory manager
	unsigned int memtotal;
	struct MEMMAN *memman  = (struct MEMMAN *)MEMMAN_ADDR;
	memtotal = memtest(0x00400000,0xbfffffff);
	memman_init(memman);
	//init memman with two mem block:0x00001000-0x0009efff and 0x00400000-memtotal
	memman_free(memman,0x00001000,0x0009e000);
	memman_free(memman,0x00400000,memtotal-0x00400000);

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
	
	//test memory size
	char mem_size[40] = {0};
	sprintf(mem_size,"memory %dMB, free %dKB.",memtotal/(1024*1024),memman_total(memman)/1024);
	putfonts8_asc(binfo->vram,binfo->scrnx,0,48,COL8_FFFFFF,mem_size);
	
	//pic enable
	io_out8(PIC0_IMR,0xf9);
	io_out8(PIC1_IMR,0xef);

	extern struct FIFO8 keybuf,mousebuf;
	char _keybuf[32] = {0};
	char _mousebuf[128] = {0};
 
	char s[50] = {0};
	fifo8_init(&keybuf,32,_keybuf);
	fifo8_init(&mousebuf,128,_mousebuf);
	for(;;)
	{
		io_cli();
		if(fifo8_status(&keybuf) + fifo8_status(&mousebuf) == 0)
		{
			io_stihlt();
		}
		else
		{
			unsigned char i;
			if(fifo8_status(&keybuf))
			{
				i = fifo8_get(&keybuf);
				io_sti();
				xtoa(i,s);
				boxfill8(binfo->vram,binfo->scrnx,COL8_000000,0,16,31,31);
				putfonts8_asc(binfo->vram,binfo->scrnx,0,16,COL8_FFFFFF,s);
			} 
			else if(fifo8_status(&mousebuf))
			{
				i = fifo8_get(&mousebuf);
				io_sti();
				if(mouse_decode(&mdec,i)!=0)
				{
					sprintf(s,"[lcr %d %d]",mdec.x,mdec.y);
					if(mdec.btn & 0x01)
						s[1] = 'L';
					if(mdec.btn & 0x02)
						s[3] = 'R';
					if(mdec.btn & 0x04)
						s[2] = 'C';
					boxfill8(binfo->vram,binfo->scrnx,COL8_008484,32,16,32+15*8-1,31);
					putfonts8_asc(binfo->vram,binfo->scrnx,32,16,COL8_FFFFFF,s);
					
					//hide old cursor
					boxfill8(binfo->vram,binfo->scrnx,COL8_008484,mx,my,mx+15,my+15);
					mx += mdec.x;
					my += mdec.y;
					if(mx<0) mx=0;
					if(my<0) my=0;
					if(mx>binfo->scrnx - 16) mx=binfo->scrnx-16;
					if(my>binfo->scrny - 16) my=binfo->scrny-16;
					sprintf(s,"(%d,%d)",mx,my);
					boxfill8(binfo->vram,binfo->scrnx,COL8_008484,0,0,79,15);
					putfonts8_asc(binfo->vram,binfo->scrnx,0,0,COL8_FFFFFF,s);
					putblock8_8(binfo->vram,binfo->scrnx,16,16,mx,my,mouse,16);
				}
			}
			
		}
	}
}
