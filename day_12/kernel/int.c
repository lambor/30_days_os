#include "int.h"
#include "io.h"
#include "graphics.h"
#include "const.h"
#include "fifo.h"
#include "timer.h"
#include "timer.h"
void init_pic(void)
{
	io_out8(PIC0_IMR, 0xff);	//disable all pic0 int
	io_out8(PIC1_IMR, 0xff);	//disable all pic1 int

	io_out8(PIC0_ICW1, 0x11);	
	io_out8(PIC0_ICW2, 0x20);
	io_out8(PIC0_ICW3, 1 << 2);
	io_out8(PIC0_ICW4, 0x01);

	io_out8(PIC1_ICW1, 0x11);
	io_out8(PIC1_ICW2, 0x28);
	io_out8(PIC1_ICW3, 2);
	io_out8(PIC1_ICW4, 0x01);

	io_out8(PIC0_IMR, 0xfb);
	io_out8(PIC1_IMR, 0xff);
}

struct FIFO8 keyfifo = {0};
//handler for int 21 (keyboard interrupts)
void inthandler21(int *esp)
{
	unsigned char data;
	//tell pic contine watch 21 int
	io_out8(PIC0_OCW2,0x61);
	data = io_in8(PORT_KEYDAT);
	fifo8_put(&keyfifo,data);
	return;
}

struct FIFO8 mousefifo = {0};
//hanlder for int 2c (mouse interrupts)
void inthandler2c(int *esp)
{
	unsigned char data;
	//tell pic continue watch 2c int
	io_out8(PIC1_OCW2,0X64);
	io_out8(PIC0_OCW2,0x62);
	data = io_in8(PORT_KEYDAT);
	fifo8_put(&mousefifo,data);
	return;
}

//handler for int 20 (timer interrupts)
void inthandler20(int *esp)
{
	io_out8(PIC0_OCW2,0x60); //tell pic continue watch 20 int
	extern struct TIMERCTL timerctl;
	timerctl.count ++;
	
	if(timerctl.count >= timerctl.next)
	{
		for(int i=0;i<timerctl.using;i++)
		{
			if(timerctl.count >= timerctl.timers[i]->timeout)
			{
				timerctl.timers[i]->flags = TIMER_FLAGS_ALLOC;
				fifo8_put(timerctl.timers[i]->fifo,timerctl.timers[i]->data);
			}
			else
			{
				timerctl.using-=i;
				//remove the timeouted timers.
				//i cannot be 0
				for(int j=0;j<timerctl.using;j++)
					timerctl.timers[j] = timerctl.timers[j+i];
				break;	
			}
		}
	}

	if(timerctl.using > 0) timerctl.next = timerctl.timers[0]->timeout;
	else timerctl.next = 0xffffffff;

	return;
}
