#include "common.h"
#include "graphics.h"
int keywin_off(struct SHEET *key_win,struct SHEET *sht_win,int cur_c,int cur_x)
{
	change_wtitle8(key_win,0);
	if(key_win == sht_win)
	{
		cur_c = -1;
		boxfill8(sht_win->buf,sht_win->bxsize,COL8_FFFFFF,cur_x,28,cur_x+7,43);
	} else {
		if((key_win->flags & 0x20))
		{
			fifo32_put(&key_win->task->fifo,3);
		}
	}
	return cur_c;
}

int keywin_on(struct SHEET *key_win,struct SHEET *sht_win,int cur_c)
{
	change_wtitle8(key_win,1);
	if(key_win == sht_win)
	{
		cur_c = COL8_000000;
	} else {
		if((key_win->flags & 0x20))
		{
			fifo32_put(&key_win->task->fifo,2);
		}
	}
	return cur_c;
}

void change_wtitle8(struct SHEET *sht,char act)
{
	int x,y,xsize = sht->bxsize;
	char c,tc_new,tbc_new,tc_old,tbc_old,*buf = sht->buf;

	if(act!=0)
	{
		tc_new = COL8_FFFFFF;
		tbc_new = COL8_000084;
		tc_old = COL8_C6C6C6;
		tbc_old = COL8_848484;
	}
	else
	{
		tc_new = COL8_C6C6C6;
		tbc_new = COL8_848484;
		tc_old = COL8_FFFFFF;
		tbc_old = COL8_000084;
	}
	for(y=3;y<=20;y++)
	{
		for(x=3;x<=xsize-4;x++)
		{
			c = buf[y*xsize + x];
			if(c == tc_old && x<=xsize -22)
			{
				c = tc_new;
			}
			else if(c == tbc_old)
			{
				c = tbc_new;
			}
			buf[y*xsize+x] = c;
		}
	}
	sheet_refresh(sht,3,3,xsize,21);
	return;
}
