#include "console.h"
#include "task.h"
#include "map_sheet.h"
int *sys_api(int edi,int esi,int ebp,int esp,int ebx,int edx,int ecx,int eax)
{
	struct CONSOLE *cons = (struct CONSOLE *) *((int *)0x0fec);
	int ds_base = *((int *)0xfe8);
	struct TASK *task = task_now();
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
		sheet_setbuf(sht,(char *)ebx+ds_base,esi,edi,eax);
		make_window8((char *)ebx+ds_base,esi,edi,(char *)ecx+ds_base,0);
		sheet_slide(sht,100,50);
		sheet_updown(sht,3);	//above task_a
		reg[7] = (int)sht;		//after sys_api ret and popad the eax will be sht
	}
	else if(edx == 6)
	{
		sht = (struct SHEET *)ebx;
		putfonts8_asc(sht->buf,sht->bxsize,esi,edi,eax,(char *)ebp + ds_base);
		sheet_refresh(sht,esi,edi,esi+ecx*8,edi+16);
	}
	else if(edx == 7)
	{
		sht = (struct SHEET *)ebx;
		boxfill8(sht->buf,sht->bxsize,ebp,eax,ecx,esi,edi);
		sheet_refresh(sht,eax,ecx,esi+1,edi+1);
	}
	return 0;
}

