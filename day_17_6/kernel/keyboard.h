#ifndef KEYBOARD_H
#define KEYBOARD_H

#define PORT_KEYDAT 0x0060
#define PORT_KEYSTA 0x0064
#define PORT_KEYCMD 0x0064
#define KEYSTA_SEND_NOTREADY 0x02
#define KEYCMD_WRITE_MODE 0x60
#define KBC_MODE 0x47

#include "fifo.h"
void wait_KBC_sendready(void);
void init_keyboard(struct FIFO32 *fifo,int data0);

#define ADR_BOOTINFO 0x00000FF0

#include "const.h"

#endif

