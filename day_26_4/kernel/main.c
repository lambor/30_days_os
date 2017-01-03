#include "header.h"


int bootmain()
{
	//the buf for all strings.
	char s[50] = {0};

	//entry.S write the boot info into 0x0ff0
	struct BOOTINFO *binfo = (struct BOOTINFO *)0xff0;

	//init fifo and keycmd fifo
	struct FIFO32 fifo,keycmd;
	int fifobuf[128], keycmd_buf[32],*cons_fifo[2];
	fifo32_init(&fifo,128,fifobuf,0);
	fifo32_init(&keycmd,32,keycmd_buf,0);

	int key_leds = (binfo->leds>>4)&7;

	//init with the keyboard led status
	fifo32_put(&keycmd,KEYCMD_LED);
	fifo32_put(&keycmd,key_leds);

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


	struct SHTCTL *shtctl;
	struct SHEET *sht_bg, *sht_mouse, *sht_console[2];
	unsigned char *buf_bg,buf_mouse[256],*buf_console[2];
	struct TASK *task_a, *task_console[2];

	init_palette();
	shtctl = shtctl_init(memman,binfo->vram,binfo->scrnx,binfo->scrny);
	*((int *)0x0fe4) = (int)shtctl;
	task_a = task_init(memman); //main task
	fifo.task = task_a;
	task_run(task_a,1,0);

	//desktop sheet
	sht_bg = sheet_alloc(shtctl);
	buf_bg = (unsigned char *)memman_alloc_4k(memman,binfo->scrnx*binfo->scrny);
	sheet_setbuf(sht_bg,buf_bg,binfo->scrnx,binfo->scrny,-1);
	init_screen8(buf_bg,binfo->scrnx,binfo->scrny);

	//mouse sheet
	sht_mouse = sheet_alloc(shtctl);
	sheet_setbuf(sht_mouse,buf_mouse,16,16,99);
	init_mouse_cursor8(buf_mouse,99);
	//mouse pos
	int mx = (binfo->scrnx-16)/2;
	int my = (binfo->scrny-28-16)/2;
	int x,y,mmx = -1,mmy = -1;

	//console sheet
	for(int i=0;i<2;i++)
	{
		sht_console[i] = sheet_alloc(shtctl);
		buf_console[i] = (unsigned char *)memman_alloc_4k(memman,256*165);
		sheet_setbuf(sht_console[i],buf_console[i],256,165,-1);
		make_window8(buf_console[i],256,165,"console",0);
		make_textbox8(sht_console[i],8,28,240,128,COL8_000000);
		task_console[i] = task_alloc();
		task_console[i]->tss.esp = memman_alloc_4k(memman,64*1024)+64*1024-12;
		task_console[i]->tss.eip = (int)&console_task - 0x280000;
		task_console[i]->tss.es = 1*8;
		task_console[i]->tss.cs = 3*8;
		task_console[i]->tss.ss = 1*8;
		task_console[i]->tss.ds = 1*8;
		task_console[i]->tss.fs = 1*8;
		task_console[i]->tss.gs = 1*8;
		*((int *)(task_console[i]->tss.esp + 4)) = sht_console[i];
		*((int *)(task_console[i]->tss.esp + 8)) = memtotal;
		task_run(task_console[i],2,2);
		sht_console[i]->task = task_console[i];
		sht_console[i]->flags |= 0x20;
		cons_fifo[i] = memman_alloc_4k(memman,128*4);
		fifo32_init(&task_console[i]->fifo,128,cons_fifo[i],task_console[i]);
	}

	sheet_slide(sht_bg,0,0);
	sheet_slide(sht_console[1],56,6);
	sheet_slide(sht_console[0],8,2);
	sheet_slide(sht_mouse,mx,my);
	sheet_updown(sht_bg,		0);
	sheet_updown(sht_console[1],1);
	sheet_updown(sht_console[0],2);
	sheet_updown(sht_mouse,		3);

	extern struct TIMERCTL timerctl;
	struct SHEET *key_win,*sht = 0;
	//init window focus
	key_win = sht_console[0];
	keywin_on(key_win);

	int key_shift = 0,keycmd_wait = -1;

	for(;;)
	{
		if(fifo32_status(&keycmd) > 0 && keycmd_wait < 0)
		{
			keycmd_wait = fifo32_get(&keycmd);
			wait_KBC_sendready();
			io_out8(PORT_KEYDAT,keycmd_wait);
		}
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
			if(key_win->flags == 0) //key_win already closed
			{
				key_win = shtctl->sheets[shtctl->top - 1];
				keywin_on(key_win);
			}
			if(256<=i && i<=511)
			{
				xtoa(i-256,s);
				putfonts8_asc_sht(sht_bg,0,16,COL8_FFFFFF,COL8_000000,s,4);

				s[0] = key_char(i-256,key_shift);
				s[1] = 0;

				if('A' <= s[0] && s[0] <= 'Z')
				{
					if((key_leds&4) == 0 && key_shift != 0 ||	//keyboard led off and shift on
							(key_leds&4) != 0 && key_shift == 0)			//keyboard led on and shift off
						s[0] += 0x20;		//uppercase 2 lowercases					
				}
				if(s[0]!=0)//visible char,return key and back key
				{
					fifo32_put(&key_win->task->fifo,s[0]+256); //send key to console fifo
				}
				if(i == 256 + 0x0f) //tab key
				{
					keywin_off(key_win);
					int j=key_win->height - 1;
					if(j==0)
					{
						j = shtctl->top-1;
					}
					key_win = shtctl->sheets[j];
					keywin_on(key_win);
				}
				else if(i == 256 + 0x2a)  //left shift on
				{
					key_shift |= 1;
				} 
				else if(i == 256 + 0x36) //right shift on
				{
					key_shift |= 2;
				}
				else if(i == 256 + 0xaa)  //left shift off
				{
					key_shift &= ~1;
				} 
				else if(i == 256 + 0xb6) //right shift off
				{
					key_shift &= ~2;
				}
				else if(i == 256 + 0x3a) //CapsLock
				{
					key_leds ^= 4;
					fifo32_put(&keycmd,KEYCMD_LED);
					fifo32_put(&keycmd,key_leds);
				}
				else if(i == 256 + 0x45) //NumLock
				{
					key_leds ^= 2;
					fifo32_put(&keycmd,KEYCMD_LED);
					fifo32_put(&keycmd,key_leds);
				}
				else if(i == 256 + 0x46) //ScrollLock
				{
					key_leds ^= 1;
					fifo32_put(&keycmd,KEYCMD_LED);
					fifo32_put(&keycmd,key_leds);
				}
				else if(i == 256 + 0xfa)
				{
					keycmd_wait = -1;
				}
				else if(i == 256 + 0xfe)
				{
					wait_KBC_sendready();
					io_out8(PORT_KEYDAT,keycmd_wait);
				}
				else if(i == 256 + 0x3b && key_shift != 0)	//shift + F1
				{
					struct TASK *task = key_win->task;
					if(task!=0 && task->tss.ss0 != 0)
					{
						extern void asm_end_app();
						cons_putstr0(task->cons,"\nBreak(key):\n");
						io_cli();
						task->tss.eax = (int)&(task->tss.esp0);
						task->tss.eip = (int)asm_end_app - 0x280000;
						io_sti();
					}
				}
				else if(i == 256 + 0x57 && shtctl->top > 2)
				{
					//F11 to pop the bottom window
					sheet_updown(shtctl->sheets[1],shtctl->top - 1);
				}

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
					sheet_slide(sht_mouse,mx,my);
					if(mdec.btn & 0x01)
					{
						if(mmx < 0) 
						{
							for(int j=shtctl->top-1;j>0;j--)
							{
								sht = shtctl->sheets[j];
								x = mx - sht->vx0;
								y = my - sht->vy0;
								if(x>=0 && x<sht->bxsize && y>=0 && y<sht->bysize)
								{
									if(sht->buf[y*sht->bxsize+x] != sht->col_inv )
									{
										sheet_updown(sht,shtctl->top-1);
										if(sht!=key_win)
										{
											keywin_off(key_win);
											key_win = sht;
											keywin_on(key_win);
										}
										if(x>=3 && x<sht->bxsize-3 && y>=3 && y<21)
										{
											mmx = mx;
											mmy = my;
										}
										if(sht->bxsize - 21 <= x && x<sht->bxsize - 5 && y>=5 && y<19) //click the close button
										{
											if(sht->flags & 0x10)
											{
												if(sht->task != 0)
												{
													struct TASK *task = sht->task;
													struct CONSOLE *cons = task->cons;
													cons_putstr0(cons,"\nBreak(mouse) :\n");
													io_cli();
													task->tss.eax = (int)&(task->tss.esp0);
													extern void asm_end_app();
													task->tss.eip = (int)&asm_end_app - 0x280000;
													io_sti();
												}
											}
										}
										break;
									}
								}
							}
						}
						else 
						{
							x = mx - mmx;
							y = my - mmy;
							sheet_slide(sht,sht->vx0+x,sht->vy0+y);
							mmx = mx;
							mmy = my;
						}
					}
					else
					{
						mmx = -1;
					}
				}
			}
		}
	}
}
