#include "header.h"


int bootmain()
{
	//buf the string.
	char s[50] = {0};
	
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
	//sheet control
	struct SHTCTL *shtctl;
	struct SHEET *sht_bg, *sht_mouse, *sht_window;
	unsigned char *buf_bg,*buf_mouse[256],*buf_window;
	
	shtctl = shtctl_init(memman,binfo->vram,binfo->scrnx,binfo->scrny);
	sht_bg = sheet_alloc(shtctl);
	sht_mouse = sheet_alloc(shtctl);
	sht_window = sheet_alloc(shtctl);
	buf_bg = (unsigned char *)memman_alloc_4k(memman,binfo->scrnx*binfo->scrny);
	buf_window = (unsigned char *)memman_alloc_4k(memman,160*68);
	sheet_setbuf(sht_bg,buf_bg,binfo->scrnx,binfo->scrny,-1);
	sheet_setbuf(sht_mouse,buf_mouse,16,16,99);
	sheet_setbuf(sht_window,buf_window,160,68,-1);
	
	test(binfo->scrnx,binfo->scrny);
	//init screen and mouse sheet
	init_screen8(buf_bg,binfo->scrnx,binfo->scrny);
	init_mouse_cursor8(buf_mouse,99);
	make_window8(buf_window,160,68,"window");
	putfonts8_asc(buf_window,160,24,28,COL8_000000,"Welcom to");
	putfonts8_asc(buf_window,160,24,44,COL8_000000,"dcnh's os");

	sheet_slide(sht_bg,0,0);
	int mx = (binfo->scrnx-16)/2;
	int my = (binfo->scrny-28-16)/2;
	sheet_slide(sht_mouse,mx,my);
	sheet_slide(sht_window,80,72);
	sheet_updown(sht_bg,0);
	sheet_updown(sht_window,1);
	sheet_updown(sht_mouse,2);
	
	putfonts8_asc(buf_bg,binfo->scrnx,8,8,COL8_FFFFFF,"ABC 123");
	putfonts8_asc(buf_bg,binfo->scrnx,31,31,COL8_000000,"dcnh os.");
	putfonts8_asc(buf_bg,binfo->scrnx,30,30,COL8_FFFFFF,"dcnh os.");
	
	//test memory size
	sprintf(s,"memory %dMB, free %dKB.",memtotal/(1024*1024),memman_total(memman)/1024);
	putfonts8_asc(buf_bg,binfo->scrnx,0,48,COL8_FFFFFF,s);
	sheet_refresh(sht_bg,0,0,binfo->scrnx,64);
	
	//pic enable
	io_out8(PIC0_IMR,0xf9);
	io_out8(PIC1_IMR,0xef);

	//keyboard mouse fifo
	extern struct FIFO8 keybuf,mousebuf;
	char _keybuf[32] = {0};
	char _mousebuf[128] = {0};
 
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
				boxfill8(buf_bg,binfo->scrnx,COL8_000000,0,16,31,31);
				putfonts8_asc(buf_bg,binfo->scrnx,0,16,COL8_FFFFFF,s);
				sheet_refresh(sht_bg,0,16,31,31);
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
					boxfill8(buf_bg,binfo->scrnx,COL8_008484,32,16,32+15*8-1,31);
					putfonts8_asc(buf_bg,binfo->scrnx,32,16,COL8_FFFFFF,s);
					
					mx += mdec.x;
					my += mdec.y;
					if(mx<0) mx=0;
					if(my<0) my=0;
					if(mx>binfo->scrnx - 1) mx=binfo->scrnx-1;
					if(my>binfo->scrny - 1) my=binfo->scrny-1;
					sprintf(s,"(%d,%d)",mx,my);
					boxfill8(buf_bg,binfo->scrnx,COL8_008484,0,0,79,15);
					putfonts8_asc(buf_bg,binfo->scrnx,0,0,COL8_FFFFFF,s);
					sheet_refresh(sht_bg,0,0,151,31);
					sheet_slide(sht_mouse,mx,my);
				}
			}
			
		}
	}
}
