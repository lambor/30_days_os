#include "header.h"


int bootmain()
{
	//the buf for all strings.
	char s[50] = {0};

	//init fifo
	struct FIFO32 fifo;
	int fifobuf[128];
	fifo32_init(&fifo,128,fifobuf,0);	

	//init gdt idt pic and enable cpu interrupt
	init_gdtidt();
	init_pic();
	io_sti();

	//init keyboard and mouse
	init_keyboard(&fifo,256);
	struct MOUSE_DEC mdec;
	enable_mouse(&fifo,512,&mdec);
	
	//init timer
	init_pit();
	
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

	struct SHTCTL *shtctl;
	struct SHEET *sht_bg, *sht_mouse, *sht_window, *sht_window_b[3];
	unsigned char *buf_bg,buf_mouse[256],*buf_window,*buf_window_b;
	struct TASK *task_a,*task_b[3];
	
	init_palette();
	shtctl = shtctl_init(memman,binfo->vram,binfo->scrnx,binfo->scrny);
	task_a = task_init(memman); //main task
	fifo.task = task_a;
	task_run(task_a,1,0);

	//desktop sheet
	sht_bg = sheet_alloc(shtctl);
	buf_bg = (unsigned char *)memman_alloc_4k(memman,binfo->scrnx*binfo->scrny);
	sheet_setbuf(sht_bg,buf_bg,binfo->scrnx,binfo->scrny,-1);
	init_screen8(buf_bg,binfo->scrnx,binfo->scrny);
	
	//task_b sheets
	for(int i=0;i<3;i++)
	{
		sht_window_b[i] = sheet_alloc(shtctl);
		buf_window_b = (unsigned char *)memman_alloc_4k(memman,144*52);
		sheet_setbuf(sht_window_b[i],buf_window_b,144,52,-1);
		sprintf(s,"task_b%d",i);
		make_window8(buf_window_b,144,52,s,0);
		task_b[i] = task_alloc();
		task_b[i]->tss.esp = memman_alloc_4k(memman,64*1024)+64*1024-8;
		task_b[i]->tss.eip = (int)&task_b_main - 0x280000;
		task_b[i]->tss.es = 1*8;
		task_b[i]->tss.cs = 3*8;
		task_b[i]->tss.ss = 1*8;
		task_b[i]->tss.ds = 1*8;
		task_b[i]->tss.fs = 1*8;
		task_b[i]->tss.gs = 1*8;
		*((int *)(task_b[i]->tss.esp + 4)) = sht_window_b[i];
		task_run(task_b[i],2,i+1);
	}
	
	//task_a window sheet
	sht_window = sheet_alloc(shtctl);
	buf_window = (unsigned char *)memman_alloc_4k(memman,160*52);
	sheet_setbuf(sht_window,buf_window,144,52,-1);
	make_window8(buf_window,144,52,"task_a",1);
	//text box in task_a window
	int cursor_x,cursor_c;
	make_textbox8(sht_window,8,28,128,16,COL8_FFFFFF);
	cursor_x = 8;
	cursor_c = COL8_FFFFFF;
	//timer for cursor.
	struct TIMER *timer;
	timer = timer_alloc();
	timer_init(timer,&fifo,1);
	timer_settime(timer,50);

	//mouse sheet
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse,buf_mouse,16,16,99);
	init_mouse_cursor8(buf_mouse,99);
	//mouse pos
	int mx = (binfo->scrnx-16)/2;
	int my = (binfo->scrny-28-16)/2;

	sheet_slide(sht_bg,0,0);
	sheet_slide(sht_window_b[0],168, 56);
	sheet_slide(sht_window_b[1],  8,116);
	sheet_slide(sht_window_b[2],168,116);
	sheet_slide(sht_window,	8, 56);
	sheet_slide(sht_mouse,mx,my);
	sheet_updown(sht_bg,0);
	sheet_updown(sht_window_b[0],1);
	sheet_updown(sht_window_b[1],2);
	sheet_updown(sht_window_b[2],3);
	sheet_updown(sht_window,4);
	sheet_updown(sht_mouse,5);
	
/*
	putfonts8_asc(buf_bg,binfo->scrnx,8,8,COL8_FFFFFF,"ABC 123");
	putfonts8_asc(buf_bg,binfo->scrnx,31,31,COL8_000000,"dcnh os.");
	putfonts8_asc(buf_bg,binfo->scrnx,30,30,COL8_FFFFFF,"dcnh os.");
*/	
	//memory size
	sprintf(s,"memory %dMB, free %dKB.",memtotal/(1024*1024),memman_total(memman)/1024);
	putfonts8_asc(buf_bg,binfo->scrnx,0,48,COL8_FFFFFF,s);
	sheet_refresh(sht_bg,0,0,binfo->scrnx,64);
	
	extern struct TIMERCTL timerctl;

	for(;;)
	{
		io_cli();
		if(fifo32_status(&fifo) == 0)
		{	
			task_sleep(task_a);
			io_sti();
		}
		else
		{
			int i = fifo32_get(&fifo);
			io_sti();
			if(256<=i && i<=511)
			{
				xtoa(i-256,s);
				putfonts8_asc_sht(sht_bg,0,16,COL8_FFFFFF,COL8_000000,s,4);

				if(i<0x54 + 256)
				{
					if(key_char(i-256)!=0 && cursor_x <144)
					{
						s[0] = key_char(i-256);
						s[1] = 0;
						putfonts8_asc_sht(sht_window,cursor_x,28,COL8_000000,COL8_FFFFFF,s,1);
						cursor_x += 8;
					}
				}
				if(i==256+0x0e && cursor_x >8) //back key
				{
					putfonts8_asc_sht(sht_window,cursor_x,28,COL8_000000,COL8_FFFFFF," ",1);
					cursor_x -= 8;
				}
				//show cursor after show character
				boxfill8(sht_window->buf,sht_window->bxsize,cursor_c,cursor_x,28,cursor_x+7,43);
				sheet_refresh(sht_window,cursor_x,28,cursor_x+8,44);
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
					if(mdec.btn & 0x01)
					{
						sheet_slide(sht_window,mx-80,my-8);
					}
				}
			}
			else if(i<=1)
			{
				if(i != 0)
				{
					timer_init(timer,&fifo,0);
					cursor_c = COL8_000000;
				}
				else
				{
					timer_init(timer,&fifo,1);
					cursor_c = COL8_FFFFFF;
				}
				boxfill8(sht_window->buf,sht_window->bxsize,cursor_c,cursor_x,28,cursor_x+7,43);
				sheet_refresh(sht_window,cursor_x,28,cursor_x+8,44);
				timer_settime(timer,50);
			}
/*
			else if(i==0)
			{
				timer_init(timer3,&fifo,1);
				boxfill8(buf_bg,binfo->scrnx,COL8_008484,8,96,15,111);
				sheet_refresh(sht_bg,8,96,16,112);
				timer_settime(timer3,50);
			}
*/				
		}
	}
}
