#include "console.h"

void cons_putchar(struct CONSOLE *cons,int chr,char move)
{
	char s[2];
	s[0] = chr;
	s[1] = 0;
	if(s[0] == 0x09)
	{
		//tab
		for(;;)
		{
			putfonts8_asc_sht(cons->sht,cons->cur_x,cons->cur_y,COL8_FFFFFF,COL8_000000," ",1);
			cons->cur_x += 8;
			if(cons->cur_x == 8+240) {
				cons_newline(cons);
			}
			if(((cons->cur_x - 8) & 0x1f) == 0)
				break;
		}
	}
	else if(s[0] == 0x0a) 
		cons_newline(cons);
	else if(s[0] == 0x0d) 
		;//todo
	else
	{
		putfonts8_asc_sht(cons->sht,cons->cur_x,cons->cur_y,COL8_FFFFFF,COL8_000000,s,1);
		if(move != 0)
		{
			cons->cur_x += 8;
			if(cons->cur_x == 8+240)
				cons_newline(cons);
		}
	}
	return;
}

void cons_newline(struct CONSOLE *cons)
{
	int x,y;
	struct SHEET *sheet = cons->sht;
	if(cons->cur_y<28+112)
	{
		cons->cur_y += 16;
	}
	else
	{
		for(y=28;y<28+112;y++)
			for(x=8;x<8+240;x++)
				sheet->buf[x+y*sheet->bxsize] = sheet->buf[x+(y+16)*sheet->bxsize];
		for(y=28+112;y<28+128;y++)
			for(x=8;x<8+240;x++)
				sheet->buf[x+y*sheet->bxsize] = COL8_000000;
		sheet_refresh(sheet,8,28,8+240,28+128);
	}
	cons->cur_x = 8;
}

#include "string.h"
void cons_runcmd(char *cmdline,struct CONSOLE *cons,int *fat,unsigned int memtotal)
{
	if(strcmp(cmdline,"mem") == 0)
		cmd_mem(cons,memtotal);
	else if(strcmp(cmdline,"cls") == 0)
		cmd_cls(cons);
	else if(strncmp(cmdline,"type",5) == 0)
		cmd_type(cons,fat,cmdline);
	else if(strcmp(cmdline,"hlt") == 0)
		cmd_hlt(cons,fat);
	else if(cmdline[0] != 0)
	{
	}
}
