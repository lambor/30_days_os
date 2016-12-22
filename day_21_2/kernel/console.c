#include "header.h"
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
	else if(strcmp(cmdline,"dir") == 0)
		cmd_dir(cons);
	else if(strncmp(cmdline,"type ",5) == 0)
		cmd_type(cons,fat,cmdline);
	//else if(strcmp(cmdline,"hlt") == 0)
	//	cmd_hlt(cons,fat);
	else if(cmdline[0] != 0)
	{
		if(cmd_app(cons,fat,cmdline) == 0)
		{
			putfonts8_asc_sht(cons->sht,8,cons->cur_y,COL8_FFFFFF,COL8_000000,"Bad command.",12);
			cons_newline(cons);
			cons_newline(cons);
		}
	}
	return;
}

void cmd_mem(struct CONSOLE *cons,unsigned int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	char s[30];
	sprintf(s,"total %dMB",memtotal/(1024*1024));
	putfonts8_asc_sht(cons->sht,8,cons->cur_y,COL8_FFFFFF,COL8_000000,s,30);
	cons_newline(cons);
	sprintf(s,"free %dKB",memman_total(memman)/1024);
	putfonts8_asc_sht(cons->sht,8,cons->cur_y,COL8_FFFFFF,COL8_000000,s,30);
	cons_newline(cons);
	cons_newline(cons);
	return;
}

void cmd_cls(struct CONSOLE *cons)
{
	int x,y;
	struct SHEET *sheet = cons->sht;
	for(y=28;y<28+128;y++)
	{
		for(x=8;x<8+240;x++)
		{
			sheet->buf[x+y*sheet->bxsize] = COL8_000000;
		}
	}
	sheet_refresh(sheet,8,28,8+240,28+128);
	cons->cur_y = 28;
	return;
}

void cmd_dir(struct CONSOLE *cons)
{
	struct FILEINFO *finfo = (struct FILEINFO *)(ADR_DISKIMG + 0x2600);
	int i,j;
	char s[30];
	for(i=0;i<224;i++)
	{
		if(finfo[i].name[0] == 0x0) break;
		if(finfo[i].name[0] != 0xe5)
		{
			if((finfo[i].type & 0x18) == 0)
			{
				sprintf(s,"filename.ext %d",finfo[i].size);
				for(j=0;j<8;j++)
				{
					s[j] = finfo[i].name[j];
				}
				s[9] = finfo[i].ext[0];
				s[10] = finfo[i].ext[1];
				s[11] = finfo[i].ext[2];
				putfonts8_asc_sht(cons->sht,8,cons->cur_y,COL8_FFFFFF,COL8_000000,s,30);
				cons_newline(cons);
			}
		}
	}
	cons_newline(cons);
	return;
}

void cmd_type(struct CONSOLE *cons,int *fat,char *cmdline)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct FILEINFO *finfo = file_search(cmdline +5,(struct FILEINFO *)(ADR_DISKIMG + 0x002600),224);
	char *p;
	int i;
	if(finfo != 0)
	{
		p = (char *)memman_alloc_4k(memman,finfo->size);
		file_loadfile(finfo->clustno,finfo->size,p,fat,(char *)(ADR_DISKIMG + 0x3e00));
		for(i=0;i<finfo->size;i++)
		{
			cons_putchar(cons,p[i],1);
		}
		memman_free_4k(memman,(int)p,finfo->size);
	}
	else
	{
		putfonts8_asc_sht(cons->sht,8,cons->cur_y,COL8_FFFFFF,COL8_000000,"File not found.",15);
		cons_newline(cons);
	}
	cons_newline(cons);
	return;
}

void cmd_hlt(struct CONSOLE *cons,int *fat)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct FILEINFO *finfo = file_search("HLT.BIN",(struct FILEINFO *)(ADR_DISKIMG + 0x2600),224);
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
	char *p;
	if(finfo != 0)
	{
		p = (char *)memman_alloc(memman,finfo->size);
		file_loadfile(finfo->clustno,finfo->size,p,fat,(char *)(ADR_DISKIMG +0x3e00));
		set_segmdesc(gdt+1003,finfo->size-1,(int)p,AR_CODE32_ER);
		farcall(0,1003*8);
		memman_free_4k(memman,(int)p,finfo->size);
	}
	else
	{
		putfonts8_asc_sht(cons->sht,8,cons->cur_y,COL8_FFFFFF,COL8_000000,"hlt not found.",15);
		cons_newline(cons);
	}
	cons_newline(cons);
	return;
}

int cmd_app(struct CONSOLE *cons,int *fat,char *cmdline)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct FILEINFO *finfo;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
	char name[18],*p;
	int i;

	for(i=0;i<13;i++)
	{
		if(cmdline[i] <= ' ')
			break;
		name[i] = cmdline[i];
	}
	name[i] = 0;

	finfo = file_search(name,(struct FILEINFO *)(ADR_DISKIMG + 0x2600),224);
	if(finfo == 0 && name[i-1]!='.')
	{
		name[i  ] = '.';
		name[i+1] = 'B';
		name[i+2] = 'I';
		name[i+3] = 'N';
		name[i+4] = 0;
		finfo = file_search(name,(struct FILEINFO *)(ADR_DISKIMG + 0x2600),224);
	}

	if(finfo != 0)
	{
		p = (char *)memman_alloc_4k(memman,finfo->size);
		*((int *)0xfe8) = (int)p;
		file_loadfile(finfo->clustno,finfo->size,p,fat,(char *)(ADR_DISKIMG + 0x3e00));
		set_segmdesc(gdt+1003,finfo->size-1,(int)p,AR_CODE32_ER);
		farcall(0,1003*8);
		memman_free_4k(memman,(int)p,finfo->size);
		cons_newline(cons);
		return 1;
	}
	return 0;
}


void cons_putstr0(struct CONSOLE *cons,char *s)
{
	for(;*s!=0;s++)
	{
		cons_putchar(cons,*s,1);
	}
	return;
}

void cons_putstr1(struct CONSOLE *cons,char *s,int l)
{
	int i;
	for(i=0;i<l;i++)
	{
		cons_putchar(cons,s[i],1);
	}
	return;
}
