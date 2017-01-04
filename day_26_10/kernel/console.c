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
			if(cons->sht!=0)	putfonts8_asc_sht(cons->sht,cons->cur_x,cons->cur_y,COL8_FFFFFF,COL8_000000," ",1);
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
		if(cons->sht!=0)	putfonts8_asc_sht(cons->sht,cons->cur_x,cons->cur_y,COL8_FFFFFF,COL8_000000,s,1);
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
	else if(sheet != 0)
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
	else if(strcmp(cmdline,"exit") == 0)
		cmd_exit(cons,fat);
	else if(strncmp(cmdline,"start ",6) == 0)
		cmd_start(cons,cmdline,memtotal);
	else if(strncmp(cmdline,"ncst ",5) == 0)
		cmd_ncst(cons,cmdline,memtotal);
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



int cmd_app(struct CONSOLE *cons,int *fat,char *cmdline)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct FILEINFO *finfo;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *)ADR_GDT;
	struct TASK *task = task_now();
	char name[18],*p,*q;
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
		file_loadfile(finfo->clustno,finfo->size,p,fat,(char *)(ADR_DISKIMG + 0x3e00));
		if(finfo->size >= 36 && strncmp(p+4,"Hari",4) == 0 && *p == 0x00)
		{
			int segsiz	=	*((int *)(p+0x0000));
			int esp		=	*((int *)(p+0x000c));
			int datsiz	=	*((int *)(p+0x0010));
			int dathrb	=	*((int *)(p+0x0014));

			q = (char *)memman_alloc_4k(memman,segsiz);
			//*((int *)0xfe8) = (int) q;
			task->ds_base = (int)q;
			set_segmdesc(gdt + task->sel/8 + 1000,finfo->size - 1,(int)p,AR_CODE32_ER + 0x60);
			set_segmdesc(gdt + task->sel/8 + 2000,segsiz - 1,     (int)q,AR_DATA32_RW + 0x60);
			for(int i = 0;i<datsiz;i++)
			{
				q[esp + i] = p[dathrb + i];
			}
			start_app(0x1b,task->sel + 1000*8,esp,task->sel + 2000*8,&(task->tss.esp0));
			struct SHEET *sht;
			struct SHTCTL *shtctl = (struct SHTCTL *) *((int *)0x0fe4);
			for(int i=0;i<MAX_SHEETS;i++)
			{
				sht = &(shtctl->sheets0[i]);
				if((sht->flags&0x11) == 0x11 && sht->task == task)
					sheet_free(sht);
			}
			timer_cancelall(&task->fifo);
			memman_free_4k(memman,(int)q,segsiz);
		}
		else
		{
			cons_putstr0(cons,".bin file format error.\n");
		}
		memman_free_4k(memman,(int)p,finfo->size);
		cons_newline(cons);
		return 1;
	}
	return 0;
}

void cmd_exit(struct CONSOLE *cons,int *fat)
{
	extern struct TASKCTL *taskctl;
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct TASK *task = task_now();
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *)0x0fe4);
	struct FIFO32 *fifo = (struct FIFO32 *) *((int *)0x0fec);
	timer_cancel(cons->timer);
	memman_free_4k(memman,(int)fat,4*2880);
	io_cli();
	if(cons->sht != 0)
		fifo32_put(fifo,cons->sht - shtctl->sheets0 + 768);
	else
		fifo32_put(fifo,task - taskctl->tasks0 + 1024 );
	io_sti();
	for(;;)
	{
		task_sleep(task);
	}
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

struct TASK *open_constask(struct SHEET *sht,unsigned int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct TASK *task = task_alloc();
	task->cons_stack = memman_alloc_4k(memman,64*1024);
	task->tss.esp = task->cons_stack+64*1024-12;
	task->tss.eip = (int)&console_task - 0x280000;
	task->tss.es = 1*8;
	task->tss.cs = 3*8;
	task->tss.ss = 1*8;
	task->tss.ds = 1*8;
	task->tss.fs = 1*8;
	task->tss.gs = 1*8;
	*((int *)(task->tss.esp + 4)) = sht;
	*((int *)(task->tss.esp + 8)) = memtotal;
	task_run(task,2,2);
	int *cons_fifo = memman_alloc_4k(memman,128*4);
	fifo32_init(&task->fifo,128,cons_fifo,task);
	return task;
}

struct SHEET *open_console(struct SHTCTL *shtctl,unsigned int memtotal)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct SHEET *sht = sheet_alloc(shtctl);
	unsigned char *buf = (unsigned char *)memman_alloc_4k(memman,256*165);
	sheet_setbuf(sht,buf,256,165,-1);
	make_window8(buf,256,165,"console",0);
	make_textbox8(sht,8,28,240,128,COL8_000000);
	sht->task = open_constask(sht,memtotal);
	sht->flags |= 0x20;
	return sht;
}

void close_constask(struct TASK *task)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	task_sleep(task);
	memman_free_4k(memman,task->cons_stack,64 * 1024);
	memman_free_4k(memman,(int)task->fifo.buf,128*4);
	task->flags = 0;
	return;
}

void close_console(struct SHEET *sht)
{
	struct MEMMAN *memman = (struct MEMMAN *)MEMMAN_ADDR;
	struct TASK *task = sht->task;
	memman_free_4k(memman,(int)sht->buf,256*165);
	sheet_free(sht);
	close_constask(task);
	return;
}

void cmd_start(struct CONSOLE *cons,char *cmdline,int memtotal)
{
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *) 0x0fe4);
	struct SHEET *sht = open_console(shtctl,memtotal);
	struct FIFO32 *fifo = &sht->task->fifo;
	int i;
	sheet_slide(sht,32,4);
	sheet_updown(sht,shtctl->top);
	for(i = 6;cmdline[i]!=0;i++)
	{
		fifo32_put(fifo,cmdline[i] + 256);
	}
	fifo32_put(fifo,10+256);
	cons_newline(cons);
	return;
}

void cmd_ncst(struct CONSOLE *cons,char *cmdline,int memtotal)
{
	struct TASK *task = open_constask(0,memtotal);
	struct FIFO32 *fifo = &task->fifo;
	int i;
	for(i=5;cmdline[i]!=0;i++) {
		fifo32_put(fifo,cmdline[i] + 256);
	}
	fifo32_put(fifo,10+256);
	cons_newline(cons);
	return;
}
