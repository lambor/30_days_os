#include "task_b.h"
#include "fifo.h"
#include "io.h"
#include "string.h"
#include "graphics.h"
void task_b_main(struct SHEET *sht_bg)
{
	struct FIFO32 fifo;
	struct TIMER *timer_put,*timer_1s;
	
	int i,fifobuf[128],count=0,count0=0;
	char s[12];

	fifo32_init(&fifo,128,fifobuf);
	timer_put = timer_alloc();
	timer_init(timer_put,&fifo,1);
	timer_settime(timer_put,1);
	timer_1s = timer_alloc();
	timer_init(timer_1s,&fifo,100);
	timer_settime(timer_1s,100);
	
	for(;;)
	{
		count ++;
		io_cli();
		if(fifo32_status(&fifo) == 0)
		{
			io_sti();
		}
		else
		{
			i = fifo32_get(&fifo);
			io_sti();
			if(i==1)
			{
				sprintf(s,"%d",count);
				putfonts8_asc_sht(sht_bg,0,144,COL8_FFFFFF,COL8_008484,s,11);
				timer_settime(timer_put,1);
				//farjmp(0,4*8);
			}
			else if(i == 100)
			{
				sprintf(s,"%d",count-count0);
				putfonts8_asc_sht(sht_bg,0,128,COL8_FFFFFF,COL8_008484,s,11);
				count0=count;
				timer_settime(timer_1s,100);
			}
		}
	}
}
