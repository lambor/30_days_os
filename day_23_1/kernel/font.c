#include "fontascii.h"
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

void putfonts8_asc(char *vram,int xsize,int x,int y,char c,unsigned char *s)
{
	extern char hankaku[];
	for(;*s !=0x00;s++)
	{
		putfont8(vram,xsize,x,y,c,hankaku+*s*HKK_TABLE_ROW);
		x+=HKK_TABLE_COL;
	}
	return; 
}
