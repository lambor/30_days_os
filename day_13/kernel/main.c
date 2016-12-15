#include "header.h"


int bootmain()
{
	struct FIFO32 fifo;
	//buf for string.
	char s[50] = {0};
	//buf for all fifo
	int fifobuf[128] = {0};
	fifo32_init(&fifo,128,fifobuf);	

	init_gdtidt();
	init_pic();
	io_sti();

	init_keyboard(&fifo,256);
	struct MOUSE_DEC mdec = {0};
	enable_mouse(&fifo,512,&mdec);
	
	//init timer
	init_pit();
	//pic enable
	io_out8(PIC0_IMR,0xf8);
	//stop the timer interrupt for debug
	//io_out8(PIC0_IMR,0xf9);
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
	unsigned char *buf_bg,buf_mouse[256],*buf_window;
	
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
	struct TIMER *timer,*timer2,*timer3;

	timer = timer_alloc();
	timer_init(timer,&fifo,10);
	timer_settime(timer,1000);
	
	timer2 = timer_alloc();
	timer_init(timer2,&fifo,3);
	timer_settime(timer2,300);

	timer3 = timer_alloc();
	timer_init(timer3,&fifo,1);
	timer_settime(timer3,50);
	
	extern struct TIMERCTL timerctl;
	extern struct FIFO32 *keyfifo,*mousefifo;
	if(keyfifo == mousefifo && mousefifo == &fifo)
		timerctl.count = 100;

	for(;;)
	{
		sprintf(s,"%d",timerctl.count);
		//boxfill8(buf_window,160,COL8_C6C6C6,24,60,104,76);
		//putfonts8_asc(buf_window,160,24,60,COL8_000000,s);
		//sheet_refresh(sht_window,24,60,104,76);
		putfonts8_asc_sht(sht_window,24,60,COL8_000000,COL8_C6C6C6,s,10);		

		io_cli();
		if(fifo32_status(&fifo) == 0)
		{	
			//not hlt
			io_sti();
		}
		else
		{
			int i = fifo32_get(&fifo);
			io_sti();
			if(256<=i && i<=511)
			{
				xtoa(i-256,s);
				//boxfill8(buf_bg,binfo->scrnx,COL8_000000,0,16,31,31);
				//putfonts8_asc(buf_bg,binfo->scrnx,0,16,COL8_FFFFFF,s);
				//sheet_refresh(sht_bg,0,16,31,31);
				putfonts8_asc_sht(sht_bg,0,16,COL8_FFFFFF,COL8_000000,s,4);
			}
			else if(512<=i && i<=767)
			{
				if(mouse_decode(&mdec,i-512)!=0)
				{
					mx += mdec.x;
					my += mdec.y;
					if(mx<0) mx=0;
					if(my<0) my=0;
					if(mx>binfo->scrnx - 1) mx=binfo->scrnx-1;
					if(my>binfo->scrny - 1) my=binfo->scrny-1;
					sprintf(s,"[lcr %d %d](%d,%d)",mdec.x,mdec.y,mx,my);
					if(mdec.btn & 0x01)
						s[1] = 'L';
					if(mdec.btn & 0x02)
						s[3] = 'R';
					if(mdec.btn & 0x04)
						s[2] = 'C';

					putfonts8_asc_sht(sht_bg,0,0,COL8_FFFFFF,COL8_008484,s,20);
					sheet_slide(sht_mouse,mx,my);
				}
			}
			else if(i == 10)
			{
				//putfonts8_asc(buf_bg,binfo->scrnx,0,64,COL8_FFFFFF,"10[sec]");
				//sheet_refresh(sht_bg,0,64,56,80);
				putfonts8_asc_sht(sht_bg,0,64,COL8_FFFFFF,COL8_008484,"10[sec]",7);
			}	
			else if(i == 3)
			{
				//putfonts8_asc(buf_bg,binfo->scrnx,0,80,COL8_FFFFFF,"3[sec]");
				//sheet_refresh(sht_bg,0,80,48,96);
				putfonts8_asc_sht(sht_bg,0,80,COL8_FFFFFF,COL8_008484,"3[sec]",6);
			}
			else if(i==1)
			{
				timer_init(timer3,&fifo,0);
				boxfill8(buf_bg,binfo->scrnx,COL8_FFFFFF,8,96,15,111);
				sheet_refresh(sht_bg,8,96,16,112);
				timer_settime(timer3,50);
			}
			else if(i==0)
			{
				timer_init(timer3,&fifo,1);
				boxfill8(buf_bg,binfo->scrnx,COL8_008484,8,96,15,111);
				sheet_refresh(sht_bg,8,96,16,112);
				timer_settime(timer3,50);
			}
				
		}
	}
}
