#include "header.h"
void console_task(struct SHEET *sheet,unsigned int memtotal)
{
	char s[50],cmdline[30];
	struct TASK *task = task_now();
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct CONSOLE cons;
	cons.sht = sheet;
	cons.cur_x = 8;
	cons.cur_y = 28;
	cons.cur_c = -1;
	//*((int *)0x0fec) = (int)&cons; //for api
	task->cons = &cons;

	int i;
	cons.timer = timer_alloc();
	timer_init(cons.timer,&task->fifo,1);
	timer_settime(cons.timer,50);
	
	int *fat = (int *)memman_alloc_4k(memman, 4*2880);
	file_readfat(fat,(unsigned char *)(ADR_DISKIMG+0x000200));

	//draw prompt char
	cons_putchar(&cons,'>',1);
	for(;;)
	{
		io_cli();
		if(fifo32_status(&task->fifo)==0)
		{
			task_sleep(task);
			io_sti();
		}
		else
		{
			i=fifo32_get(&task->fifo);
			io_sti();
			if(i<=1) //for cursor
			{
				if(i!=0)
				{
					timer_init(cons.timer,&task->fifo,0);
					if(cons.cur_c >= 0) cons.cur_c = COL8_FFFFFF;
				}
				else
				{
					timer_init(cons.timer,&task->fifo,1);
					if(cons.cur_c >= 0) cons.cur_c = COL8_000000;
				}
				timer_settime(cons.timer,50);
							
			}
			else if(i == 2) //get show cursor msg
			{
				cons.cur_c = COL8_FFFFFF;
			}
			else if(i == 3) //get hide cursor msg
			{
				cons.cur_c = -1;
				boxfill8(sheet->buf,sheet->bxsize,COL8_000000,cons.cur_x,28,cons.cur_x+7,43);
			}
			else if(256<=i && i<=511)
			{
				if(i == 8+256) //back key
				{
					if(cons.cur_x > 16)
					{
						cons_putchar(&cons,' ',0);
						cons.cur_x -= 8;
					}
				}
				else if(i == 10+256) //return key
				{
					cons_putchar(&cons,' ',0);
					cmdline[cons.cur_x/8-2] = 0;
					cons_newline(&cons);
					cons_runcmd(cmdline,&cons,fat,memtotal);
					cons_putchar(&cons,'>',1);
				}
				else
				{
					if(cons.cur_x<240)
					{
						s[0] = i-256;
						s[1] = 0;
						cmdline[cons.cur_x/8-2] = i-256;
						cons_putchar(&cons,i-256,1);
					}
				}
			}
			if(cons.cur_c >= 0) boxfill8(sheet->buf,sheet->bxsize,cons.cur_c,cons.cur_x,cons.cur_y,cons.cur_x+7,cons.cur_y+15);
			sheet_refresh(sheet,cons.cur_x,cons.cur_y,cons.cur_x+8,cons.cur_y+16);

		}
	}
}


