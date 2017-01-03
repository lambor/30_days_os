#include "timer.h"
#include "int.h"
#include "task.h"

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
	t->flags = TIMER_FLAGS_USING;
	timerctl.t0 = t;
	timerctl.next = 0xffffffff;
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
			timerctl.timers0[i].flags2= 0;
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

void timer_init(struct TIMER *timer,struct FIFO32 *fifo,int data)
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
	if(timer->timeout<=timerctl.t0->timeout)
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
		if(timer->timeout<=t->timeout)
		{
			timer->next = t;
			s->next = timer;
			io_store_eflags(e);
			return;
		}
		s = t;
		t = t->next;
	}
	io_store_eflags(e);
	return;
}

extern struct TIMER *task_timer;

//handler for int 20 (timer interrupts)
void inthandler20(int *esp)
{
	char ts = 0;

	io_out8(PIC0_OCW2,0x60); //tell pic continue watch 20 int
	timerctl.count ++;
	
	if(timerctl.count >= timerctl.next)
	{
		struct TIMER *t = timerctl.t0;
		while(t)
		{
			if(t->timeout <= timerctl.count)
			{
				t->flags = TIMER_FLAGS_ALLOC;
				if(t!=task_timer) fifo32_put(t->fifo,t->data);
				else ts=1;
				t = t->next;
			}
			else
			{
				timerctl.next = t->timeout;
				timerctl.t0 = t;
				break;
			}
		}
	}

	if(ts!=0)task_switch();
	return;
}

int timer_cancel(struct TIMER *timer)
{
	int e;
	struct TIMER *t;
	e = io_load_eflags();
	io_cli();
	if(timer->flags == TIMER_FLAGS_USING)
	{
		if(timer == timerctl.t0)
		{
			t = timer->next;
			timerctl.t0 = t;
			timerctl.next = t->timeout;
		}
		else
		{
			t = timerctl.t0;
			for(;;)
			{
				if(t->next == timer)
				{
					break;
				}
				t = t->next;
			}
			t->next = timer->next;
		}
		timer->flags = TIMER_FLAGS_ALLOC;
		io_store_eflags(e);
		return 1;
	}
	io_store_eflags(e);
	return 0;
}

void timer_cancelall(struct FIFO32 *fifo)
{
	int e,i;
	struct TIMER *t;
	e = io_load_eflags();
	io_cli();
	for(i=0;i<MAX_TIMER;i++)
	{
		t = &timerctl.timers0[i];
		if(t->flags != 0 && t->flags2 != 0 && t->fifo == fifo)
		{
			timer_cancel(t);
			timer_free(t);
		}
	}
	io_store_eflags(e);
	return;
}
