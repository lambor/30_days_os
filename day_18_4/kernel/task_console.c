#include "task_console.h"
#include "graphics.h"
#include "const.h"
#include "fifo.h"
#include "timer.h"
#include "task_console.h"
void console_task(struct SHEET *sheet)
{
	char s[50];
	struct TIMER *timer;
	struct TASK *task = task_now();
	
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
					if(cursor_y<28+112)
					{
						cursor_y += 16;
					}
					else
					{
						int x,y;
						for(y = 28;y < 28+112;y++)
							for(x = 8;x < 8+240;x++)
								sheet->buf[x+y*sheet->bxsize] = sheet->buf[x+(y+16)*sheet->bxsize];
						for(y = 28+112;y < 28+128;y++)
							for(x = 8;x < 8+240;x++)
								sheet->buf[x+y*sheet->bxsize] = COL8_000000;
						sheet_refresh(sheet,8,28,8+240,28+128);
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
