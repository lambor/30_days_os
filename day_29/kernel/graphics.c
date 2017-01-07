#include "graphics.h"

void init_palette(void)
{
	static unsigned char table_rgb[16*3] = {
		0x00, 0x00, 0x00,	//black
		0xff, 0x00, 0x00,	//red
		0x00, 0xff, 0x00,	//green
		0xff, 0xff, 0x00,	//yellow
		0x00, 0x00, 0xff,	//blue 
		0xff, 0x00, 0xff,	//purple
		0x00, 0xff, 0xff,	//light blue
		0xff, 0xff, 0xff,	//white
		0xc6, 0xc6, 0xc6,	//gray 
		0x84, 0x00, 0x00,	//dark red
		0x00, 0x84, 0x00,	//dark green
		0x84, 0x84, 0x00,	//dark yellow
		0x00, 0x00, 0x84,	//dark blue
		0x84, 0x00, 0x84,	//dark purple
		0x00, 0x84, 0x84,	//dark light blue
		0x84, 0x84, 0x84,	//dark gray
	};
	unsigned char table2[216*3];
	int r,g,b;
	set_palette(0,15,table_rgb);
	for(b=0;b<6;b++)
	{
		for(g=0;g<6;g++)
		{
			for(r=0;r<6;r++)
			{
				table2[(r+g*6+b*36)*3+0] = r*51;
				table2[(r+g*6+b*36)*3+1] = g*51;
				table2[(r+g*6+b*36)*3+2] = b*51;
			}
		}
	}
	set_palette(16,231,table2);
	return;	
}

void set_palette(int start, int end, unsigned char *rgb)
{
	int i,eflags;
	eflags = io_load_eflags();
	io_cli();
	io_out8(0x03c8,start);
	for(i=start;i<=end;i++)
	{
		io_out8(0x03c9,rgb[0]/4);
		io_out8(0x03c9,rgb[1]/4);
		io_out8(0x03c9,rgb[2]/4);
		rgb+=3;
	}
	io_store_eflags(eflags);
	return;
}

void  boxfill8(unsigned char *vram,int xsize,unsigned char c,int x0,int y0,int x1,int y1)
{
	int x,y;
	for(y=y0;y<=y1;y++)
	{
		for(x=x0;x<=x1;x++)
			vram[y*xsize+x]=c;
	}
		
	return;
}

void boxfill8_test(unsigned char *vram,unsigned char c,int size)
{
	int i;
	for(i=0;i<size;i++)
	{
		vram[i] = c;
	}
	//vram[TRY_BG_SIZE-1] = c;
}

void init_screen8(unsigned char *vram, int xsize, int ysize)
{
	boxfill8(vram,310,COL8_008484,0,0,310,190);
	boxfill8(vram,xsize,COL8_008484,0,      0,          xsize-1,    ysize-29);
	boxfill8(vram,xsize,COL8_C6C6C6,0,      ysize-28,   xsize-1,    ysize-28);
	boxfill8(vram,xsize,COL8_FFFFFF,0,      ysize-27,   xsize-1,    ysize-27);
	boxfill8(vram,xsize,COL8_C6C6C6,0,      ysize-26,   xsize-1,    ysize- 1);
	
	
	boxfill8(vram,xsize,COL8_FFFFFF,3,      ysize-24,   59,         ysize-24);
	boxfill8(vram,xsize,COL8_FFFFFF,2,      ysize-24,    2,         ysize- 4);
	boxfill8(vram,xsize,COL8_848484,3,      ysize- 4,   59,         ysize- 4);
	boxfill8(vram,xsize,COL8_848484,59,     ysize-23,   59,         ysize- 5);
	boxfill8(vram,xsize,COL8_000000,2,      ysize- 3,   59,         ysize- 3);
	boxfill8(vram,xsize,COL8_000000,60,     ysize-24,   60,         ysize- 3);
	
	boxfill8(vram,xsize,COL8_848484,xsize-47,       ysize-24,   xsize- 4,   ysize-24);
	boxfill8(vram,xsize,COL8_848484,xsize-47,       ysize-23,   xsize-47,   ysize- 4);
	boxfill8(vram,xsize,COL8_FFFFFF,xsize-47,       ysize- 3,   xsize- 4,   ysize- 3);
	boxfill8(vram,xsize,COL8_FFFFFF,xsize- 3,       ysize-24,   xsize- 3,   ysize- 3);
}


void init_mouse_cursor8(char *mouse, char bc)
{
	static char cursor[16][16] = {
		"**************..",
        "*OOOOOOOOOOO*...",
        "*OOOOOOOOOO*....",
        "*OOOOOOOOO*.....",
        "*OOOOOOOO*......",
        "*OOOOOOO*.......",
        "*OOOOOOO*.......",
        "*OOOOOOOO*......",
        "*OOOO**OOO*.....",
        "*OOO*..*OOO*....",
        "*OO*....*OOO*...",
        "*O*......*OOO*..",
        "**........*OOO*.",
        "*..........*OOO*",
        "............*OO*",
        ".............***"
	};
	int x,y;
	
	for(y=0;y<16;y++)
	{
		for(x=0;x<16;x++)
		{
			if(cursor[y][x] == '*')
				mouse[y*16+x] = COL8_000000;
			else if(cursor[y][x] == 'O')
				mouse[y*16+x] = COL8_FFFFFF;
			else if(cursor[y][x] == '.')
				mouse[y*16+x] = bc;
		}
	}
	return;
}

void make_window8(unsigned char *buf,int xsize,int ysize,char *title,char act)
{
	boxfill8(buf, xsize, COL8_C6C6C6, 0     , 0     , xsize - 1, 0  );
    boxfill8(buf, xsize, COL8_FFFFFF, 1     , 1     , xsize - 2, 1  );
    boxfill8(buf, xsize, COL8_C6C6C6, 0     , 0     , 0     , ysize-1);
    boxfill8(buf, xsize, COL8_FFFFFF, 1     , 1     , 1     , ysize-2);
    boxfill8(buf, xsize, COL8_848484, xsize-2,1     , xsize-2,ysize-2);
    boxfill8(buf, xsize, COL8_000000, xsize-1,0     , xsize-1,ysize-1);
    boxfill8(buf, xsize, COL8_C6C6C6, 2     , 2     , xsize-3,ysize-3);
    boxfill8(buf, xsize, COL8_848484, 1     , ysize-2,xsize-2,ysize-2);
    boxfill8(buf, xsize, COL8_000000, 0     , ysize-1,xsize-1,ysize-1);
	make_wtitle8(buf,xsize,title,act);
	return;
}

void make_wtitle8(unsigned char *buf,int xsize,char *title,char act)
{

	static char closebtn[14][16] = {
			"OOOOOOOOOOOOOOO@",
			"OQQQQQQQQQQQQQ$@",
			"OQQQQQQQQQQQQQ$@",
			"OQQQ@@QQQQ@@QQ$@",
			"OQQQQ@@QQ@@QQQ$@",
			"OQQQQQ@@@@QQQQ$@",
			"OQQQQQQ@@QQQQQ$@",
			"OQQQQQ@@@@QQQQ$@",
			"OQQQQ@@QQ@@QQQ$@",
			"OQQQ@@QQQQ@@QQ$@",
			"OQQQQQQQQQQQQQ$@",
			"OQQQQQQQQQQQQQ$@",
			"O$$$$$$$$$$$$$$@",
			"@@@@@@@@@@@@@@@@"
	};
	int x,y;
	char c,tc,tbc;
	if(act!=0)
	{
		tc = COL8_FFFFFF;
		tbc= COL8_000084;
	}
	else
	{
		tc = COL8_C6C6C6;
		tbc= COL8_848484;
	}
	//draw title bar
    boxfill8(buf, xsize, tbc		, 3     , 3     , xsize-4,20    );
	putfonts8_asc(buf,xsize,24,4,tc,title);
	
	//draw close button
	for(y=0;y<14;y++)
	{
		for(x=0;x<16;x++)
		{
			c=closebtn[y][x];
			if(c == '@') c=COL8_000000;
			else if(c == '$') c=COL8_848484;
			else if(c == 'Q') c=COL8_C6C6C6;
			else c=COL8_FFFFFF;
			buf[(5+y)*xsize+(xsize-21+x)] = c;
		}
	}
	return;
}

void putblock8_8(char *vram,int vxsize,int pxsize,int pysize,int px0,int py0,char *buf,int bxsize)
{
	int x,y;
	for(y=0;y<pysize;y++)
		for(x=0;x<pxsize;x++)
			vram[(py0+y)*vxsize+(px0+x)]=buf[y*bxsize+x];

	return;
}
//b background color
//c font color
void putfonts8_asc_sht(struct SHEET *sht,int x,int y,int c,int b,char *s,int l)
{
	boxfill8(sht->buf,sht->bxsize,b,x,y,x+l*8-1,y+15);
	putfonts8_asc(sht->buf,sht->bxsize,x,y,c,s);
	sheet_refresh(sht,x-8,y,x+l*8,y+16); //x-8 for hanyu refresh
}

void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c)
{
    int x1 = x0 + sx, y1 = y0 + sy;
    boxfill8(sht->buf, sht->bxsize, COL8_848484, x0 - 2, y0 - 3, x1 + 1, y0 - 3);
    boxfill8(sht->buf, sht->bxsize, COL8_848484, x0 - 3, y0 - 3, x0 - 3, y1 + 1);
    boxfill8(sht->buf, sht->bxsize, COL8_FFFFFF, x0 - 3, y1 + 2, x1 + 1, y1 + 2);
    boxfill8(sht->buf, sht->bxsize, COL8_FFFFFF, x1 + 2, y0 - 3, x1 + 2, y1 + 2);
    boxfill8(sht->buf, sht->bxsize, COL8_000000, x0 - 1, y0 - 2, x1 + 0, y0 - 2);
    boxfill8(sht->buf, sht->bxsize, COL8_000000, x0 - 2, y0 - 2, x0 - 2, y1 + 0);
    boxfill8(sht->buf, sht->bxsize, COL8_C6C6C6, x0 - 2, y1 + 1, x1 + 0, y1 + 1);
    boxfill8(sht->buf, sht->bxsize, COL8_C6C6C6, x1 + 1, y0 - 2, x1 + 1, y1 + 1);
    boxfill8(sht->buf, sht->bxsize, c,           x0 - 1, y0 - 1, x1 + 0, y1 + 0);
    return;
}

void drawline8_sht(struct SHEET *sht,int x0,int y0,int x1,int y1,int col)
{
	int i,x,y,len,dx,dy;
	dx = x1-x0;
	dy = y1-y0;
	x = x0 << 10;
	y = y0 << 10;
	if (dx<0) 
	{
		dx = -dx;
	}
	if (dy<0) 
	{
		dy = -dy;
	}
	if(dx >= dy)
	{
		len=dx+1;
		if(x0 >x1)
			dx = -1024;
		else
			dx = 1024;
		if(y0<=y1)
			dy = ((y1 - y0 + 1)<<10)/len;
		else
			dy = ((y1 - y0 - 1)<<10)/len;
	}
	else
	{
		len=dy+1;
		if(y0>y1)
			dy = -1024;
		else
			dy = 1024;
		if(x0<=x1)
			dx = ((x1 - x0 + 1)<<10)/len;
		else
			dx = ((x1 - x0 - 1)<<10)/len;
	}
	for(i=0;i<len;i++)
	{
		sht->buf[(y>>10)*sht->bxsize+(x>>10)] = col;
		x += dx;
		y += dy;
	}
	return;
}
