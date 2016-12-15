#include "keyboard.h"

struct FIFO32 *keyfifo;
int keydata0;

void wait_KBC_sendready(void)
{
	for(;;)
	{
		if((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0)
			break;
	}
	return;
}

void init_keyboard(struct FIFO32 *fifo,int data0)
{
	keyfifo = fifo;
	keydata0 = data0;
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD,KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT,KBC_MODE);
	return;
}

#include "io.h"
#include "const.h"
#include "fifo.h"


//handler for int 21 (keyboard interrupts)
void inthandler21(int *esp)
{
	int data;
	//tell pic contine watch 21 int
	io_out8(PIC0_OCW2,0x61);
	data = io_in8(PORT_KEYDAT);
	fifo32_put(keyfifo,data + keydata0);
	return;
}
