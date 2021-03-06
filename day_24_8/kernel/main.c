#include "header.h"


int bootmain()
{
	//the buf for all strings.
	char s[50] = {0};

	//entry.S write the boot info into 0x0ff0
	struct BOOTINFO *binfo = (struct BOOTINFO *)0xff0;

	//init fifo and keycmd fifo
	struct FIFO32 fifo,keycmd;
	int fifobuf[128], keycmd_buf[32];
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
	struct SHEET *sht_bg, *sht_mouse, *sht_window, *sht_console;
	unsigned char *buf_bg,buf_mouse[256],*buf_window,*buf_console;
	struct TASK *task_a, *task_console;

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
	int x,y,mmx = -1,mmy = -1;

	//console sheet
	sht_console = sheet_alloc(shtctl);
	buf_console = (unsigned char *)memman_alloc_4k(memman,256*165);
	sheet_setbuf(sht_console,buf_console,256,165,-1);
	make_window8(buf_console,256,165,"console",0);
	make_textbox8(sht_console,8,28,240,128,COL8_000000);
	task_console = task_alloc();
	task_console->tss.esp = memman_alloc_4k(memman,64*1024)+64*1024-12;
	task_console->tss.eip = (int)&console_task - 0x280000;
	task_console->tss.es = 1*8;
	task_console->tss.cs = 3*8;
	task_console->tss.ss = 1*8;
	task_console->tss.ds = 1*8;
	task_console->tss.fs = 1*8;
	task_console->tss.gs = 1*8;
	*((int *)(task_console->tss.esp + 4)) = sht_console;
	*((int *)(task_console->tss.esp + 8)) = memtotal;
	task_run(task_console,2,2);


	sheet_slide(sht_bg,0,0);
	sheet_slide(sht_console,32,4);
	sheet_slide(sht_window,	64, 56);
	sheet_slide(sht_mouse,mx,my);
	sheet_updown(sht_bg,0);
	sheet_updown(sht_console,1);
	sheet_updown(sht_window,2);
	sheet_updown(sht_mouse,3);

	extern struct TIMERCTL timerctl;
	struct SHEET *key_win,*sht = 0;

	//init window focus
	key_win = sht_window;
	sht_console->task = task_console;
	sht_console->flags |= 0x20;

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
				cursor_c = keywin_on(key_win,sht_window,cursor_c);
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
				if(s[0]!=0)//visible char
				{
					if(key_win == sht_window && cursor_x <128)
					{
						putfonts8_asc_sht(sht_window,cursor_x,28,COL8_000000,COL8_FFFFFF,s,1);
						cursor_x += 8;
					}
					else
					{
						fifo32_put(&key_win->task->fifo,s[0]+256); //send key to console fifo
					}

				}
				if(i == 256 + 0x0f) //tab key
				{
					cursor_c = keywin_off(key_win,sht_window,cursor_c,cursor_x);
					int j=key_win->height - 1;
					if(j==0)
					{
						j = shtctl->top-1;
					}
					key_win = shtctl->sheets[j];
					cursor_c = keywin_on(key_win,sht_window,cursor_c);
				}
				else if(i == 256 + 0x0e) //back key
				{
					if(key_win == sht_window && cursor_x >8)
					{
						putfonts8_asc_sht(sht_window,cursor_x,28,COL8_000000,COL8_FFFFFF," ",1);
						cursor_x -= 8;
					}
					else
					{
						fifo32_put(&key_win->task->fifo,8+256);
					}
				}
				else if(i == 256 + 0x1c)  //return key
				{
					if(key_win != sht_window) //forcus on task_console window
					{
						fifo32_put(&key_win->task->fifo,10+256);
					}
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
				else if(i == 256 + 0x3b && key_shift != 0 && task_console->tss.esp0 != 0)	//shift + F1
				{
					struct CONSOLE *cons = (struct CONSOLE *) *((int *)0x0fec);
					extern void asm_end_app();
					cons_putstr0(cons,"\nBreak(key):\n");
					io_cli();
					task_console->tss.eax = (int)&(task_console->tss.esp0);
					task_console->tss.eip = (int)asm_end_app - 0x280000;
					io_sti();
				}
				else if(i == 256 + 0x57 && shtctl->top > 2)
				{
					//F11 to pop the bottom window
					sheet_updown(shtctl->sheets[1],shtctl->top - 1);
				}
				if(cursor_c >= 0)
				{
					//show cursor after show character
					boxfill8(sht_window->buf,sht_window->bxsize,cursor_c,cursor_x,28,cursor_x+7,43);
				}
				sheet_refresh(sht_window,cursor_x,28,cursor_x+8,44);
				sprintf(s,"%d,%d",key_leds,key_shift);
				putfonts8_asc_sht(sht_bg,0,32,COL8_FFFFFF,COL8_000000,s,4);

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
											cursor_c = keywin_off(key_win,sht_window,cursor_c,cursor_x);
											key_win = sht;
											cursor_c = keywin_on(key_win,sht_window,cursor_c);
										}
										if(x>=3 && x<sht->bxsize-3 && y>=3 && y<21)
										{
											mmx = mx;
											mmy = my;
										}
										if(sht->bxsize - 21 <= x && x<sht->bxsize - 5 && y>=5 && y<19)
										{
											if(sht->flags & 0x10)
											{
												if(sht->task != 0)
												{
													struct CONSOLE *cons = (struct CONSOLE *) *((int *)0x0fec);
													cons_putstr0(cons,"\nBreak(mouse) :\n");
													io_cli();
													task_console->tss.eax = (int)&(task_console->tss.esp0);
													extern void asm_end_app();
													task_console->tss.eip = (int)&asm_end_app - 0x280000;
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
			else if(i<=1)
			{
				if(i != 0)
				{
					timer_init(timer,&fifo,0);
					if(cursor_c>=0) cursor_c = COL8_000000;
				}
				else
				{
					timer_init(timer,&fifo,1);
					if(cursor_c>=0) cursor_c = COL8_FFFFFF;
				}
				if(cursor_c >= 0) // check if the color invalid
				{
					boxfill8(sht_window->buf,sht_window->bxsize,cursor_c,cursor_x,28,cursor_x+7,43);
					sheet_refresh(sht_window,cursor_x,28,cursor_x+8,44);
				}
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
