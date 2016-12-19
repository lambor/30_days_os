#include "task.h"
#include "timer.h"

struct TIMER *mt_timer;
int mt_tr;

void mt_init(void)
{
	mt_timer = timer_alloc();
	timer_settime(mt_timer,2);
	mt_tr = 4*8;
	return;
}

void mt_taskswitch(void)
{
	if(mt_tr == 4*8)
	{
		mt_tr = 5*8;
	}
	else
	{
		mt_tr = 4*8;
	}
	timer_settime(mt_timer,2);
	farjmp(0,mt_tr);
	return;
}
