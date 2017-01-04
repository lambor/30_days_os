#include "header.h"
int *sys_api(int edi,int esi,int ebp,int esp,int ebx,int edx,int ecx,int eax)
{
	struct TASK *task = task_now();
	struct CONSOLE *cons = task->cons;
	int ds_base = task->ds_base;
	struct SHTCTL *shtctl = (struct SHTCTL *) *((int *)0x0fe4);
	struct SHEET *sht;
	int *reg = &eax + 1;
	/*
	   before call sys_api using pushad twice.
	   reg[0] = 1st_edi; reg[1] = 1st_esi ... reg[7] = 1st_eax
	   and the parameters:
	   edi = 2nd_edi; esi = 2nd_esi ... eax = 2nd_eax
	 */

	if(edx == 1) cons_putchar(cons,eax & 0xff,1);
	else if(edx == 2) cons_putstr0(cons,(char *)ebx+ds_base);
	else if(edx == 3) cons_putstr1(cons,(char *)ebx+ds_base,ecx);
	else if(edx == 4)
	{
		//exit app
		return &(task->tss.esp0);
	}
	else if(edx == 5)
	{
		sht = sheet_alloc(shtctl);
		sht->task = task;
		sht->flags |= 0x11;
		sheet_setbuf(sht,(char *)ebx+ds_base,esi,edi,eax);
		make_window8((char *)ebx+ds_base,esi,edi,(char *)ecx+ds_base,0);
		sheet_slide(sht,((shtctl->xsize - esi)/2)&~3,(shtctl->ysize - edi)/2);
		sheet_updown(sht,shtctl->top);	//above task_a
		reg[7] = (int)sht;		//after sys_api ret and popad the eax will be sht
	}
	else if(edx == 6)
	{
		sht = (struct SHEET *)ebx;
		putfonts8_asc(sht->buf,sht->bxsize,esi,edi,eax,(char *)ebp + ds_base);
		if(!(ebx&1))
			sheet_refresh(sht,esi,edi,esi+ecx*8,edi+16);
	}
	else if(edx == 7)
	{
		sht = (struct SHEET *)ebx;
		boxfill8(sht->buf,sht->bxsize,ebp,eax,ecx,esi,edi);
		if(!(ebx&1))
			sheet_refresh(sht,eax,ecx,esi+1,edi+1);
	}
	else if(edx == 8)
	{
		memman_init((struct MEMMAN *)(ebx + ds_base));
		ecx &= 0xfffffff0;
		memman_free((struct MEMMAN *)(ebx + ds_base),eax,ecx);
	}
	else if(edx == 9)
	{
		ecx = (ecx + 0x0f) & 0xfffffff0;
		reg[7] = memman_alloc((struct MEMMAN *) (ebx + ds_base), ecx);
	}
	else if(edx == 10)
	{
		ecx = (ecx + 0x0f) & 0xfffffff0;
		memman_free((struct MEMMAN *)(edx + ds_base),eax,ecx);
	}
	else if(edx == 11)
	{
		sht = (struct SHEET *)ebx;
		sht->buf[sht->bxsize * edi + esi] = eax;
		if(!(ebx&1))
			sheet_refresh(sht,esi,edi,esi+1,edi+1);
	}
	else if(edx == 12)
	{
		sht = (struct SHEET *)ebx;
		sheet_refresh(sht,eax,ecx,esi,edi);
	}
	else if(edx == 13)
	{
		sht = (struct SHEET *)(ebx & 0xfffffffe);
		drawline8_sht(sht,eax,ecx,esi,edi,ebp);
		if(!(ebx&1))
			sheet_refresh(sht,eax,ecx,esi+1,edi+1);
	}
	else if(edx == 14)
	{
		sheet_free((struct SHEET *)ebx);
	}
	else if(edx == 15)
	{
		for(;;)
		{
			io_cli();
			if(fifo32_status(&task->fifo) == 0)
			{
				if(eax != 0)
					task_sleep(task);
				else
				{
					io_sti();
					reg[7] = -1;
					return 0;
				}
			}
			int i = fifo32_get(&task->fifo);
			io_sti();
			if(i <= 1)
			{
				timer_init(cons->timer,&task->fifo,1);
				timer_settime(cons->timer,50);
			}
			else if(i == 2)
			{
				cons->cur_c = COL8_FFFFFF;
			}
			else if(i == 3)
			{
				cons->cur_c = -1;
			}
			else if(i>=256)
			{
				reg[7] = i-256;
				return 0;
			}
		}
	}
	else if(edx == 16)
	{
		reg[7] = (int)timer_alloc();
		((struct TIMER *)reg[7])->flags2 = 1;
	}
	else if(edx == 17)
	{
		timer_init((struct TIMER *)ebx,&task->fifo,eax+256);
	}
	else if(edx == 18)
	{
		timer_settime((struct TIMER *)ebx,eax);
	}
	else if(edx == 19)
	{
		timer_free((struct TIMER *)ebx);
	}
	else if(edx == 20)
	{
		if(eax == 0)
		{
			int i = io_in8(0x61);
			io_out8(0x61,i&0x0d);
		}
		else
		{
			int i = 1193180000/eax;
			io_out8(0x43,0xb6);
			io_out8(0x42,i&0xff);
			io_out8(0x42,i>>8);
			i=io_in8(0x61);
			io_out8(0x61,(i|0x03)&0x0f);
		}
	}
	return 0;
}

