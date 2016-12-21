#include "header.h"
void console_task(struct SHEET *sheet,unsigned int memtotal)
{
	char s[50],cmdline[30];
	struct TIMER *timer;
	struct TASK *task = task_now();
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct FILEINFO *finfo = (struct FILEINFO *)(ADR_DISKIMG+0x002600); //0x002600 the offset of directory entry in the os.img
	
	int i,fifobuf[128],cursor_x = 16,cursor_y = 28,cursor_c = -1;
	fifo32_init(&task->fifo,128,fifobuf,task);
	timer = timer_alloc();
	timer_init(timer,&task->fifo,1);
	timer_settime(timer,50);
	
	//draw prompt char
	putfonts8_asc_sht(sheet,8,28,COL8_FFFFFF,COL8_000000,">",1);

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
					timer_init(timer,&task->fifo,0);
					if(cursor_c >= 0) cursor_c = COL8_FFFFFF;
				}
				else
				{
					timer_init(timer,&task->fifo,1);
					if(cursor_c >= 0) cursor_c = COL8_000000;
				}
				timer_settime(timer,50);
							
			}
			else if(i == 2) //get show cursor msg
			{
				cursor_c = COL8_FFFFFF;
			}
			else if(i == 3) //get hide cursor msg
			{
				cursor_c = -1;
				boxfill8(sheet->buf,sheet->bxsize,COL8_000000,cursor_x,28,cursor_x+7,43);
			}
			else if(256<=i && i<=511)
			{
				if(i == 8+256) //back key
				{
					if(cursor_x > 16)
					{
						putfonts8_asc_sht(sheet,cursor_x,cursor_y,COL8_FFFFFF,COL8_000000," ",1);
						cursor_x -= 8;
					}
				}
				else if(i == 10+256) //return key
				{
					
					putfonts8_asc_sht(sheet,cursor_x,cursor_y,COL8_FFFFFF,COL8_000000," ",1); //erase the cursor
					cmdline[cursor_x/8-2] = 0;
					cursor_y = cons_newline(cursor_y,sheet);
					if(strcmp(cmdline,"mem")==0)
					{
						//mem cmd
						sprintf(s,"total %dMB",memtotal/(1024*1024));
						putfonts8_asc_sht(sheet,8,cursor_y,COL8_FFFFFF,COL8_000000,s,30);
						cursor_y = cons_newline(cursor_y,sheet);
						sprintf(s,"free %dKB",memman_total(memman)/1024);
						putfonts8_asc_sht(sheet,8,cursor_y,COL8_FFFFFF,COL8_000000,s,30);
						cursor_y = cons_newline(cursor_y,sheet);
						cursor_y = cons_newline(cursor_y,sheet);
					}
					else if(strcmp(cmdline,"cls")==0)
					{
						for(int y=28;y<8+128;y++)
							for(int x=8;x<8+240;x++)
								sheet->buf[x+y*sheet->bxsize] = COL8_000000;
						sheet_refresh(sheet,8,28,8+240,28+128);
						cursor_y = 28;
					}
					else if(strcmp(cmdline,"dir")==0)
					{
						for(int x=0;x<224;x++)
						{
							if(finfo[x].name[0] == 0) break;
							if(finfo[x].name[0] != 0xe5 &&(finfo[x].type & 0x18) == 0)
							{
								sprintf(s,"filename.ext %d",finfo[x].size);
								for(int y=0;y<8;y++)
								{
									s[y] = finfo[x].name[y];
								}
								s[9]=finfo[x].ext[0];
								s[10]=finfo[x].ext[1];
								s[11]=finfo[x].ext[2];
								putfonts8_asc_sht(sheet,8,cursor_y,COL8_FFFFFF,COL8_000000,s,30);
								cursor_y = cons_newline(cursor_y,sheet);
							}
						}
					}
					else if(cmdline[0]!=0)
					{
						putfonts8_asc_sht(sheet,8,cursor_y,COL8_FFFFFF,COL8_000000,"Bad command.",12);
						cursor_y = cons_newline(cursor_y,sheet);
						cursor_y = cons_newline(cursor_y,sheet);
					}
					putfonts8_asc_sht(sheet,8,cursor_y,COL8_FFFFFF,COL8_000000,">",1);
					cursor_x = 16;
				}
				else
				{
					if(cursor_x<240)
					{
						s[0] = i-256;
						s[1] = 0;
						cmdline[cursor_x/8-2] = i-256;
						putfonts8_asc_sht(sheet,cursor_x,cursor_y,COL8_FFFFFF,COL8_000000,s,1);
						cursor_x += 8;
					}
				}
			}
			if(cursor_c >= 0) boxfill8(sheet->buf,sheet->bxsize,cursor_c,cursor_x,cursor_y,cursor_x+7,cursor_y+15);
			sheet_refresh(sheet,cursor_x,cursor_y,cursor_x+8,cursor_y+16);

		}
	}
}

int cons_newline(int cursor_y,struct SHEET *sheet)
{
	int x,y;
	if(cursor_y<28+112)
	{
		cursor_y += 16;
	}
	else
	{
		for(y=28;y<28+112;y++)
			for(x=8;x<8+240;x++)
				sheet->buf[x+y*sheet->bxsize] = sheet->buf[x+(y+16)*sheet->bxsize];
		for(y=28+112;y<28+128;y++)
			for(x=8;x<8+240;x++)
				sheet->buf[x+y*sheet->bxsize] = COL8_000000;
		sheet_refresh(sheet,8,28,8+240,28+128);
	}
	return cursor_y;
}
