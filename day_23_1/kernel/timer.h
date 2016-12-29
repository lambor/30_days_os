#ifndef timer_h
#define timer_h

#define PIT_CTRL 0x043
#define PIT_CNT0 0x040

void init_pit(void);

#include "fifo.h"

struct TIMER 
{
	struct TIMER *next;
	unsigned int timeout,flags;
	struct FIFO *fifo;
	unsigned char data;
};

#define MAX_TIMER 500

struct TIMERCTL 
{
	unsigned int count,next;
	struct TIMER timers0[MAX_TIMER];
	struct TIMER *t0;
};

#define TIMER_FLAGS_ALLOC 1 //already alloced
#define TIMER_FLAGS_USING 2 //using
struct TIMER *timer_alloc(void);

void timer_free(struct TIMER *timer);

void timer_init(struct TIMER *timer,struct FIFO32* fifo,unsigned char data);

void timer_settime(struct TIMER *timer,unsigned int timeout);

#endif
