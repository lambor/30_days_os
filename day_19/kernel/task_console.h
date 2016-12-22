#ifndef task_console_h
#define task_console_h
#include "map_sheet.h"
void console_task(struct SHEET *sheet,unsigned int memtotal);
int cons_newline(int cursor_y,struct SHEET *sheet);

struct FILEINFO {
	char slot[32]; //my file sys type is vfat
	unsigned char name[8],ext[3],type;
	char reserve[10];
	unsigned short time,date,clustno;
	unsigned int size;
};

#endif
