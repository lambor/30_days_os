#include "timer.h"

struct TIMERCTL timerctl;

void init_pit(void)
{
	io_out8(PIT_CTRL,0X34);
	//out 0x2e9c (11932), make the pit freq = 100hz
	io_out8(PIT_CNT0,0X9C);
	io_out8(PIT_CNT0,0x2e);
	for(int i=0;i<MAX_TIMER;i++)
		timerctl.timers0[i].flags = 0;
	struct TIMER *t = timer_alloc();
	t->next = 0;
	t->timeout = 0xffffffff;
	t->flags = TIMER_FLAG_USING;
	timerctl.t0 = t;
	timerctl.next = 0xffffffff;
	timerctl.using = 1;
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
	struct TIMER *t, *s;
	if(timer.timeout<=timerctl.to->timeout)
	{
		timer->next = timerctl.t0;
		timerctl.t0 = timer;
		timerctl.next = timer->timeout;
		io_store_eflags(e);
		return;
	}
	t = timerctl.t0->next;
	s = timerctl.t0;
	while(t)
	{
		if(timer.timeout<=t->timeout)
		{
			timer->next = t;
			s->next = timer;
			io_store_eflags(e);
			return;
		}
		s = t;
		t = t->next
	}
	io_store_eflags(e);
	return;
}
