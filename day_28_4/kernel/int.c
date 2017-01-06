#include "int.h"
#include "io.h"
#include "graphics.h"
#include "const.h"
#include "fifo.h"
#include "timer.h"
#include "string.h"
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

//when app code execute or access sys code or mem
int *inthandler0d(int *esp)
{
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	cons_putstr0(cons,"\nINT 0D:\n General Protected Exception.\n");
	char s[30];
	sprintf(s,"EIP=%x\n",esp[11]);
	cons_putstr0(cons,s);
	return &(task->tss.esp0);
}


//when code write beyond stack too much
int *inthandler0c(int *esp)
{
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	cons_putstr0(cons,"\nINT 0C:\n Stack Exception.\n");
	char s[30];
	sprintf(s,"EIP=%x\n",esp[11]);
	cons_putstr0(cons,s);
	return &(task->tss.esp0);
}

