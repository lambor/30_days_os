#include "fifo.h"
void fifo32_init(struct FIFO32 *fifo,int size,int *buf,struct TASK *task)
{
	fifo->size=size;
	fifo->buf=buf;
	fifo->free = size;
	fifo->flags = 0;
	fifo->p = 0;
	fifo->q = 0;
	fifo->task = task;
	return;
}

#define FLAGS_OVERRUN 0x0001
#include "timer.h"
extern struct TIMERCTL timerctl;

int fifo32_put(struct FIFO32 *fifo,int data)
{
	if(fifo->free == 0)
	{
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if(fifo->p == fifo->size)
		fifo->p = 0;
	fifo->free--;

	//wake up task
	if(fifo->task!=0)
	{
		if(fifo->task->flags!=2)
		{
			task_run(fifo->task);
		}
	}
	return 0;
}

int fifo32_get(struct FIFO32 *fifo)
{
	if(fifo->free == fifo->size)
		return -1;
	int data = fifo->buf[fifo->q];
	fifo->q++;
	if(fifo->q == fifo->size)
		fifo->q = 0;
	fifo->free++;
	return data;
}

int fifo32_status(struct FIFO32 *fifo)
{
	return fifo->size - fifo->free;
}

