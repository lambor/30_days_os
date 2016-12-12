#ifndef header_h
#define header_h

void io_hlt();
void io_cli();
void io_sti();
void io_stihlt();

void write_mem8(int addr,int data);

int io_in8(int port);
int io_in16(int port);
int io_in32(int port);

void io_out8(int port,int data);
void io_out16(int port,int data);
void io_out32(int port,int data);

int io_load_eflags();
void io_store_eflags(int eflags);

#define COL8_000000 0
#define COL8_FF0000 1
#define COL8_00FF00 2
#define COL8_FFFF00 3
#define COL8_0000FF 4
#define COL8_FF00FF 5
#define COL8_00FFFF 6
#define COL8_FFFFFF 7
#define COL8_C6C6C6 8
#define COL8_840000 9
#define COL8_008400 10
#define COL8_848400 11
#define COL8_000084 12
#define COL8_840084 13
#define COL8_008484 14
#define COL8_848484 15

void init_palette(void);
void set_palette(int start,int end,unsigned char *rgb);

void boxfill8(unsigned char *vram,int xsize,unsigned char c,int x0,int y0,int x1,int y1);

struct BOOTINFO {
	char cyls,leds,vmode,reserve;
	short scrnx,scrny;
	char *vram;
};

void init_screen(char *vram,int xsize,int ysize);


void putfonts8_asc(char *vram,int xsize,int x,int y,char c,unsigned char *s);
#endif
