#include "fontascii.h"
#include "task.h"

#define HZ_TABLE_COL 16
#define HZ_TABLE_ROW 16

void putfont8(char *vram, int xsize,int x,int y,char c,unsigned char *font)
{
	int i;
	char *p;
	unsigned char d;
	for(i=0;i<HKK_TABLE_ROW;i++)
	{
		p = vram + (y+i)*xsize+x;
		d = font[i];
		int col = HKK_TABLE_COL-1;
		for(;col>=0;col--)
		{
			if(d&0x1) p[col] = c;
			d=d>>1;
		}
	}

}

void putfont16(char *vram, int xsize,int x,int y,char c,unsigned char *font)
{
	int i;
	char *p;
	unsigned char d,b;
	for(i=0;i<HZ_TABLE_ROW;i++)
	{
		p = vram + (y+i)*xsize+x;
		d = font[2*i];
		b = font[2*i+1];
		int col = HZ_TABLE_COL/2-1;
		for(;col>=0;col--)
		{
			if(d&0x1) p[col] = c;
			d=d>>1;
			if(b&0x1) p[col+8] = c;
			b=b>>1;
		}
	}

}

void putfonts8_asc(char *vram,int xsize,int x,int y,char c,unsigned char *s)
{
	extern char hankaku[];
	struct TASK *task = task_now();
	char *hanyu = (char *)*((int *)0x0fe8);
	if(task->langmode == 0)
	{
		for(;*s !=0x00;s++)
		{
			putfont8(vram,xsize,x,y,c,hankaku+*s*HKK_TABLE_ROW);
			x+=HKK_TABLE_COL;
		}
	}
	else if(task->langmode == 1)
	{
		for(;*s != 0x00;s++)
		{
			if(task->langbyte1 == 0)
			{
				if(0x81<=*s && *s<=0xfe)
					task->langbyte1 = *s;
				else
					putfont8(vram,xsize,x,y,c,hanyu+*s*16);
			}
			else
			{
				int k=task->langbyte1 - 0xa1;
				int t=*s - 0xa1;
				task->langbyte1 = 0;
				char *font = hanyu + 256 *16 + (k*94+t)*32;
				putfont16(vram,xsize,x-8,y,c,font);
				//x+=HZ_TABLE_COL;
			}
			x+=HZ_TABLE_COL/2;
		}
	}
	return; 
}
