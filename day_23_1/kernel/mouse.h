#ifndef MOUSE_H
#define MOUSE_H

#define KEYCMD_SENDTO_MOUSE 0xd4
#define MOUSECMD_ENABLE 0xf4

#include "fifo.h"

struct MOUSE_DEC {
	unsigned char buf[3], phase;
	int x,y,btn;
};

void enable_mouse(struct FIFO32 *fifo,int data0,struct MOUSE_DEC *mdec);
int mouse_decode(struct MOUSE_DEC *mdec,unsigned char dat);

#include "const.h"

#include "keyboard.h" //some common in keyboard.h

#endif

