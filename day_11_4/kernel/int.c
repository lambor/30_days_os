#include "int.h"
#include "io.h"
#include "graphics.h"
#include "const.h"
#include "fifo.h"
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

struct FIFO8 keybuf = {0};
//handler for int 21 (keyboard interrupts)
void inthandler21(int *esp)
{
	unsigned char data;
	io_out8(PIC0_OCW2,0x61);
	data = io_in8(PORT_KEYDAT);
	fifo8_put(&keybuf,data);
	return;
}

struct FIFO8 mousebuf = {0};
//hanlder for int 2c (mouse interrupts)
void inthandler2c(int *esp)
{
	unsigned char data;
	io_out8(PIC1_OCW2,0X64);
	io_out8(PIC0_OCW2,0x62);
	data = io_in8(PORT_KEYDAT);
	fifo8_put(&mousebuf,data);
	return;
}

