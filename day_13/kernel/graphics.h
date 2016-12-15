#ifndef graphics_h
#define graphics_h

#include "const.h"

void init_palette(void);
void set_palette(int start,int end,unsigned char *rgb);

void boxfill8(unsigned char *vram,int xsize,unsigned char c,int x0,int y0,int x1,int y1);

struct BOOTINFO {
	char cyls,leds,vmode,reserve;
	short scrnx,scrny;
	char *vram;
};

void init_screen8(unsigned char *vram,int xsize,int ysize);

void init_mouse_cursor8(char *mouse,char bc);

void make_window8(unsigned char *buf,int xsize,int ysize,char *title);

void putblock8_8(char *vram,int vxsize,int pxsize,int pysize,int px0,int py0,char *buf,int bxsize);

#include "map_sheet.h"
void putfonts8_asc_sht(struct SHEET *sht,int x,int y,int c,int b,char *s,int l);
#endif
