#include "header.h"


int bootmain()
{
	//buf for string.
	char s[50] = {0};
	
	init_gdtidt();
	init_pic();
	io_sti();

	init_keyboard();
	struct MOUSE_DEC mdec = {0};
	enable_mouse(&mdec);

	//keyboard mouse fifo
	extern struct FIFO8 keyfifo,mousefifo;
	char _keybuf[32] = {0};
	char _mousebuf[128] = {0};
	fifo8_init(&keyfifo,32,_keybuf);
	fifo8_init(&mousefifo,128,_mousebuf);
	
	//init timer
	init_pit();
	//pic enable
	io_out8(PIC0_IMR,0xf8);
	io_out8(PIC1_IMR,0xef);


	//init memory manager
	unsigned int memtotal,count = 0;
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
	buf_window = (unsigned char *)memman_alloc_4k(memman,160*100);
	sheet_setbuf(sht_bg,buf_bg,binfo->scrnx,binfo->scrny,-1);
	sheet_setbuf(sht_mouse,buf_mouse,16,16,99);
	sheet_setbuf(sht_window,buf_window,160,100,-1);
	
	test(binfo->scrnx,binfo->scrny);
	//init screen and mouse sheet
	init_screen8(buf_bg,binfo->scrnx,binfo->scrny);
	init_mouse_cursor8(buf_mouse,99);
	make_window8(buf_window,160,100,"window");
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
	
	//timers
	struct FIFO8 timerfifo,timerfifo2,timerfifo3;
	char timerbuf[8],timerbuf2[8],timerbuf3[8];
	struct TIMER *timer,*timer2,*timer3;

	fifo8_init(&timerfifo,8,timerbuf);
	timer = timer_alloc();
	timer_init(timer,&timerfifo,1);
	timer_settime(timer,1000);
	
	fifo8_init(&timerfifo2,8,timerbuf2);
	timer2 = timer_alloc();
	timer_init(timer2,&timerfifo2,1);
	timer_settime(timer2,300);

	fifo8_init(&timerfifo3,8,timerbuf3);
	timer3 = timer_alloc();
	timer_init(timer3,&timerfifo3,1);
	timer_settime(timer3,50);
	
	extern struct TIMERCTL timerctl;
	for(;;)
	{
		sprintf(s,"%d",timerctl.count);
		boxfill8(buf_window,160,COL8_C6C6C6,24,60,104,76);
		putfonts8_asc(buf_window,160,24,60,COL8_000000,s);
		sheet_refresh(sht_window,24,60,104,76);		

		io_cli();
		if(fifo8_status(&keyfifo) + fifo8_status(&mousefifo) 
			+fifo8_status(&timerfifo) + fifo8_status(&timerfifo2) + fifo8_status(&timerfifo3) == 0)
		{
			//io_stihlt();
			io_sti();
		}
		else
		{
			unsigned char i;
			if(fifo8_status(&keyfifo))
			{
				i = fifo8_get(&keyfifo);
				io_sti();
				xtoa(i,s);
				boxfill8(buf_bg,binfo->scrnx,COL8_000000,0,16,31,31);
				putfonts8_asc(buf_bg,binfo->scrnx,0,16,COL8_FFFFFF,s);
				sheet_refresh(sht_bg,0,16,31,31);
			}
			else if(fifo8_status(&mousefifo))
			{
				i = fifo8_get(&mousefifo);
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
			else if(fifo8_status(&timerfifo))
			{
				i = fifo8_get(&timerfifo);
				io_sti();
				putfonts8_asc(buf_bg,binfo->scrnx,0,64,COL8_FFFFFF,"10[sec]");
				sheet_refresh(sht_bg,0,64,56,80);
			}	
			else if(fifo8_status(&timerfifo2))
			{
				i = fifo8_get(&timerfifo2);
				io_sti();
				putfonts8_asc(buf_bg,binfo->scrnx,0,80,COL8_FFFFFF,"3[sec]");
				sheet_refresh(sht_bg,0,64,48,96);
			}
			else if(fifo8_status(&timerfifo3))
			{
				i = fifo8_get(&timerfifo3);
				io_sti();
				if(i!=0)
				{
					timer_init(timer3,&timerfifo3,0);
					boxfill8(buf_bg,binfo->scrnx,COL8_FFFFFF,8,96,15,111);
				}
				else
				{
					timer_init(timer3,&timerfifo3,1);
					boxfill8(buf_bg,binfo->scrnx,COL8_008484,8,96,15,111);
				}
				timer_settime(timer3,50);
				sheet_refresh(sht_bg,8,96,16,112);
			}

		}
	}
}
