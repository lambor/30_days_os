#include "int.h"
#include "io.h"
#include "graphics.h"
#include "const.h"
#include "fifo.h"
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

