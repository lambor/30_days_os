#include "header.h"
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
	set_palette(0,15,table_rgb);
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
		for(x=x0;x<=x1;x++)
			vram[y*xsize+x]=c;

	return;
}

void init_screen(char *vram, int xsize, int ysize)
{
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

void putblock8_8(char *vram,int vxsize,int pxsize,int pysize,int px0,int py0,char *buf,int bxsize)
{
	int x,y;
	for(y=0;y<pysize;y++)
		for(x=0;x<pxsize;x++)
			vram[(py0+y)*vxsize+(px0+x)]=buf[y*bxsize+x];

	return;
}

struct SEGMENT_DESCRIPTOR
{
	short limit_low,base_low;
	char base_mid,access_right;
	char limit_high,base_high;
};

struct GATE_DESCRIPTOR
{
	short offset_low,selector;
	char dw_count,access_right;
	short offset_high;
};



void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit,int base,int ar)
{
	if(limit>0xfffff)
	{
		ar |= 0x8000;
		limit /= 0x1000;
	}
	sd->limit_low = limit&0xffff;
	sd->base_low = base&0xffff;
	sd->base_mid = (base>>16) & 0xff;
	sd->access_right = ar & 0xff;
	sd->limit_high = ((limit>>16)&0x0f) | ((ar>>8)&0xf0);
	sd->base_high = (base>>24)&0xff;
	return;
}

void set_gatedesc(struct GATE_DESCRIPTOR *gd,int offset,int selector,int ar)
{
	gd->offset_low = offset & 0xffff;
	gd->selector = selector;
	gd->dw_count = (ar>>8) & 0xff;
	gd->access_right = ar&0xff;
	gd->offset_high = (offset>>16)&0xffff;
	return;
}

void init_gdtidt(void)
{
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) 0x00270000;
	struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *) 0x0026f800;
	int i;

	for(i=0;i<8192;i++)
	{
		set_segmdesc(gdt+i,0,0,0);
	}
	set_segmdesc(gdt+1,0xffffffff,0x00000000,0x4092);
	set_segmdesc(gdt+2,0x0007ffff,0x00280000,0x409a);
	load_gdtr(0xffff,0x00270000);

	for(i=0;i<256;i++)
	{
		set_gatedesc(idt+i,0,0,0);
	}
	load_idtr(0x7ff,0x0026f800);

	return;
}
