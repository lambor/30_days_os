#include "timer.h"

struct TIMERCTL timerctl;

void init_pit(void)
{
	io_out8(PIT_CTRL,0X34);
	//out 0x2e9c (11932), make the pit freq = 100hz
	io_out8(PIT_CNT0,0X9C);
	io_out8(PIT_CNT0,0x2e);
	timerctl.count = 0;
	timerctl.using = 0;
	timerctl.next = 0xffffffff;
	for(int i=0;i<MAX_TIMER;i++)
		timerctl.timers0[i].flags = 0;
	return;
}

struct TIMER *timer_alloc(void)
{
	int i;
	for(i=0;i<MAX_TIMER;i++)
	{
		if(timerctl.timers0[i].flags == 0)
		{
			timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
			return &timerctl.timers0[i];
		}
	}
	return 0;
}

void timer_free(struct TIMER *timer)
{
	timer->flags = 0;
	return;
}

void timer_init(struct TIMER *timer,struct FIFO8 *fifo,unsigned char data)
{
	timer->fifo = fifo;
	timer->data = data;
	return ;
}

void timer_settime(struct TIMER *timer,unsigned int timeout)
{
	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_FLAGS_USING;
	int e = io_load_eflags();
	io_cli();
	int i = 0;
	for(;i<timerctl.using;i++)
	{
		if(timerctl.timers[i]->timeout>=timer->timeout)
		{
			for(int j=timerctl.using;j>i;j--)
			{
				timerctl.timers[j] = timerctl.timers[j-1];
			}
			break;
		}
	}
	//outside of the loop for using=0
	timerctl.timers[i] = timer;
	timerctl.using ++;
	timerctl.next = timerctl.timers[0]->timeout;
	io_store_eflags(e);
	return;
}
