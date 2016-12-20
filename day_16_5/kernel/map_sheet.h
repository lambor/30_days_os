#ifndef map_sheet_h
#define map_sheet_h

#define MAX_SHEETS 256

struct SHEET
{
	unsigned char *buf;
	int bxsize,bysize,vx0,vy0,col_inv,height,flags;
	//col_inv: transparent color inverse value(
	//height: index of sheet in SHTCTL->sheets[]
	struct SHTCTL *ctl;
};

struct SHTCTL
{
	unsigned char *vram, *map;
	int xsize,ysize,top;
	struct SHEET *sheets[MAX_SHEETS]; //pointers to sheets0[]
	struct SHEET sheets0[MAX_SHEETS];
};

#include "memory.h"
struct SHTCTL* shtctl_init(struct MEMMAN *man,unsigned char *vram,int xsize,int ysize);

#define SHEET_USE 1 //flag for used sheet

struct SHEET *sheet_alloc(struct SHTCTL *ctl);

void sheet_setbuf(struct SHEET *sht,unsigned char *buf,int xsize,int ysize,int col_inv);

void sheet_updown(struct SHEET *sht,int height);

void sheet_refresh(struct SHEET *sht,int bx0,int by0,int bx1,int by1);

void sheet_slide(struct SHEET *sht,int vx0,int vy0);

void sheet_free(struct SHEET *sht);

#endif
